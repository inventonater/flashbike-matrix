#include <Adafruit_Protomatter.h>
#include <stdlib.h> // For random numbers
#include "Wiichuck.h"

#define WIDTH 64
#define HEIGHT 64
#define FRAME_DELAY 100

const int POINTS_PER_LINE = 100;
const int MAX_SCORE = 1000;
const int MAX_DELAY = 1000;
const int MIN_DELAY = 100;

uint8_t rgbPins[] = {
    42,
    41,
    40,
    38,
    39,
    37,
};
uint8_t addrPins[] = {
    45,
    36,
    48,
    35,
    21};

uint8_t clockPin = 2;
uint8_t latchPin = 47;
uint8_t oePin = 14;

Adafruit_Protomatter matrix(WIDTH, 6, 1, rgbPins, sizeof(addrPins) / sizeof(addrPins[0]), addrPins, clockPin, latchPin, oePin, false);
Accessory chuck;

void renderGame();

struct Point
{
    int x, y;
};

struct Tetromino
{
    Point blocks[4]; // Relative positions of blocks
    int rotation;
    Point position; // Position on the board
    uint16_t color; // Color
};

uint16_t board[WIDTH][HEIGHT] = {0};
Tetromino tetrominoes[7] = {
    {{{0, 0}, {0, 1}, {0, 2}, {0, 3}}, 0, {0, 0}, 0xFFFF},  // I
    {{{0, 0}, {0, 1}, {0, 2}, {1, 2}}, 0, {0, 0}, 0xFF00},  // L
    {{{0, 0}, {0, 1}, {0, 2}, {-1, 2}}, 0, {0, 0}, 0xF81F}, // J
    {{{0, 0}, {0, 1}, {1, 0}, {1, 1}}, 0, {0, 0}, 0xF800},  // O
    {{{0, 0}, {0, 1}, {0, 2}, {1, 1}}, 0, {0, 0}, 0x07E0},  // T
    {{{0, 0}, {0, 1}, {1, 1}, {1, 2}}, 0, {0, 0}, 0x001F},  // S
    {{{0, 0}, {0, 1}, {-1, 1}, {-1, 2}}, 0, {0, 0}, 0xF800} // Z
};

int gameOverPattern[7][35] = {
    {0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1}, // G  A  M  E     O  V  E  R
    {0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0}, // G  A  M  E     O  V  E  R
    {0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1}, // G  A  M  E     O  V  E  R
    {0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0}, // G  A  M  E     O  V  E  R
    {0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0}, // G  A  M  E     O  V  E  R
};


Tetromino getRandomTetromino() {
    int index = random(7); // Get a random index between 0 and 6
    return tetrominoes[index];
}

Tetromino currentPiece;
Tetromino nextPiece;

bool gameOver = false;
int score = 0;

bool canPlace(Tetromino piece)
{
    // Check each block of the Tetromino
    for (auto &block : piece.blocks)
    {
        int x = piece.position.x + block.x;
        int y = piece.position.y + block.y;

        // Check if the block is out of bounds
        if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        {
            return false;
        }

        // Check if the block collides with an existing block on the board
        if (board[x][y] != 0)
        {
            return false;
        }
    }

    // If none of the blocks are out of bounds or colliding, the Tetromino can be placed
    return true;
}

bool canMove(Tetromino piece, int dx, int dy)
{
    // Create a copy of the Tetromino and move it
    Tetromino movedPiece = piece;
    movedPiece.position.x += dx;
    movedPiece.position.y += dy;

    // Check if the moved Tetromino can be placed
    return canPlace(movedPiece);
}


void selectRandomTetromino()
{
    // The next piece becomes the current piece
    currentPiece = nextPiece;

    // Generate a new next piece
    nextPiece = getRandomTetromino();

    // Set the position of the current piece to the top of the board
    currentPiece.position = {WIDTH / 2, 0};

    // If the new piece can't be placed, the game is over
    if (!canPlace(currentPiece))
    {
        gameOver = true;
    }
}

void rotateTetromino(bool clockwise)
{
    // Create a copy of the Tetromino and rotate it
    Tetromino rotatedPiece = currentPiece;
    for (auto &block : rotatedPiece.blocks)
    {
        int oldX = block.x;
        if (clockwise)
        {
            block.x = block.y;
            block.y = -oldX;
        }
        else
        {
            block.x = -block.y;
            block.y = oldX;
        }
    }

    // Try to place the rotated Tetromino
    if (canPlace(rotatedPiece))
    {
        currentPiece = rotatedPiece;
    }
    else
    {
        // Try wall kick
        for (int dx : {1, -1, 2, -2})
        {
            rotatedPiece.position.x += dx;
            if (canPlace(rotatedPiece))
            {
                currentPiece = rotatedPiece;
                break;
            }
            rotatedPiece.position.x -= dx; // Reset position if wall kick failed
        }
    }
}

void handleInput()
{
    // Update the Wiichuck state

    chuck.readData(); // Read inputs and update maps
    Serial.print("Joy: (");
    Serial.print(chuck.getJoyX());
    Serial.print(", ");
    Serial.print(chuck.getJoyY());
    Serial.print(")");
    Serial.println();

    Serial.print("X: ");
    Serial.print(chuck.getAccelX());
    Serial.print(" \tY: ");
    Serial.print(chuck.getAccelY());
    Serial.print(" \tZ: ");
    Serial.print(chuck.getAccelZ());
    Serial.println();

    Serial.print("Button: ");
    if (chuck.getButtonZ())
        Serial.print(" Z ");
    if (chuck.getButtonC())
        Serial.print(" C ");

    Serial.println();

    // Move the current Tetromino based on the Wiichuck joystick
    if (chuck.getJoyX() < 0)
    {
        // Move left
        if (canMove(currentPiece, -1, 0))
        {
            currentPiece.position.x--;
        }
    }
    else if (chuck.getJoyX() > 0)
    {
        // Move right
        if (canMove(currentPiece, 1, 0))
        {
            currentPiece.position.x++;
        }
    }
    else if (chuck.getJoyY() > 0)
    {
        // Fall faster
        if (canMove(currentPiece, 0, 1))
        {
            currentPiece.position.y++;
        }
    }

    // Rotate the current Tetromino based on the Wiichuck buttons
    if (chuck.getButtonZ())
    {
        // Rotate counterclockwise
        rotateTetromino(false);
    }
    else if (chuck.getButtonC())
    {
        // Rotate clockwise
        rotateTetromino(true);
    }
}

void lockTetromino()
{
    // Add each block of the Tetromino to the board
    for (auto &block : currentPiece.blocks)
    {
        int x = currentPiece.position.x + block.x;
        int y = currentPiece.position.y + block.y;

        // Set the color of the block on the board
        board[x][y] = currentPiece.color;
    }
}

bool isLineFull(int y) {
    for (int x = 0; x < WIDTH; x++) {
        if (board[x][y] == 0) {
            return false;
        }
    }
    return true;
}

void flashLine(int y) {
    for (int i = 0; i < 3; i++) {
        // Turn off the line
        for (int x = 0; x < WIDTH; x++) {
            board[x][y] = 0;
        }
        renderGame();
        delay(100);

        // Turn on the line
        for (int x = 0; x < WIDTH; x++) {
            board[x][y] = 0xFFFF; // White color
        }
        renderGame();
        delay(100);
    }
}

void moveLinesDown(int y) {
    for (int y2 = y; y2 > 0; y2--) {
        for (int x = 0; x < WIDTH; x++) {
            board[x][y2] = board[x][y2 - 1];
        }
    }
}

void clearTopLine() {
    for (int x = 0; x < WIDTH; x++) {
        board[x][0] = 0;
    }
}

void clearLines() {
    int linesCleared = 0;

    for (int y = HEIGHT - 1; y >= 0; y--) {
        if (isLineFull(y)) {
            flashLine(y);
            moveLinesDown(y);
            clearTopLine();
            linesCleared++;
        }
    }

    score += linesCleared * POINTS_PER_LINE;
}

void displayScreen(uint16_t bgColor, int cursorX, int cursorY, uint16_t textColor, const char* text) {
    matrix.fillScreen(bgColor); // Fill the screen with the specified color
    matrix.setCursor(cursorX, cursorY); // Set the cursor to the specified position
    matrix.setTextColor(textColor); // Set the text color
    matrix.println(text); // Print the text
    matrix.show(); // Update the screen
}

void displayIntroScreen() {
    displayScreen(0x07E0, 0, 0, 0xFFFF, "Tetris Game");
}

void displayGameOverScreen() {
    displayScreen(0xF800, 0, 0, 0xFFFF, "Game Over");
}

void initGame()
{
    // Initialize the Wiichuck
    chuck.begin();
	if (chuck.type == Unknown) {
		chuck.type = NUNCHUCK;
	}

    // Initialize the game board to be empty
    for (int x = 0; x < WIDTH; x++)
    {
        for (int y = 0; y < HEIGHT; y++)
        {
            board[x][y] = 0;
        }
    }

    // Select the first Tetromino
    selectRandomTetromino();

    // Set the score to 0
    score = 0;

    // Set the game over flag to false
    gameOver = false;
}

void updateGame()
{
    // Try to move the current Tetromino down
    if (canMove(currentPiece, 0, 1))
    {
        currentPiece.position.y++;
    }
    else
    {
        // If the Tetromino can't be moved down, lock it to the board
        lockTetromino();

        // Clear any full lines
        clearLines();

        // Spawn a new Tetromino
        selectRandomTetromino();
    }
}

void renderGame()
{
    matrix.fillScreen(0); // Clear the screen

    // Draw the board and the current tetromino
    for (int x = 0; x < WIDTH; x++)
    {
        for (int y = 0; y < HEIGHT; y++)
        {
            if (board[x][y] != 0)
            {
                matrix.drawPixel(x, y, board[x][y]);
            }
        }
    }

    // Example: Draw the current piece
    for (auto &block : currentPiece.blocks)
    {
        int x = currentPiece.position.x + block.x;
        int y = currentPiece.position.y + block.y;
        matrix.drawPixel(x, y, currentPiece.color);
    }

    // Draw the next piece in a dedicated area of the screen
    for (auto &block : nextPiece.blocks)
    {
        int x = WIDTH + block.x; // Adjust the x-coordinate to the dedicated area
        int y = block.y;
        matrix.drawPixel(x, y, nextPiece.color);
    }

    // Draw the score meter
    int scoreHeight = (score / MAX_SCORE) * HEIGHT; // Calculate the height of the score meter
    for (int y = 0; y < scoreHeight; y++)
    {
        matrix.drawPixel(WIDTH - 1, HEIGHT - 1 - y, 0xFFFF); // Draw the score meter in the last column
    }

    matrix.show(); // Copy data to matrix buffers
}

void setup() {
    Serial.begin(115200);
    if (!matrix.begin()) {
        Serial.println("Failed to start the matrix");
        while (1);
    }

    // Display the intro screen
    displayIntroScreen();
    delay(2000); // Wait for 2 seconds

    // Initialize your tetrominoes and game state here
    initGame();
}

void loop()
{
    if (gameOver) {
        displayGameOverScreen();
        delay(2000); // Wait for 2 seconds
        displayIntroScreen();
        delay(2000); // Wait for 2 seconds
        initGame(); // Restart the game
        return;
    }
    handleInput();
    updateGame();
    clearLines();
    renderGame();

    // Calculate the current delay based on the score
    int currentDelay = MAX_DELAY - ((score / (float)MAX_SCORE) * (MAX_DELAY - MIN_DELAY));
    delay(currentDelay); // Slow down the game speed based on the score
}