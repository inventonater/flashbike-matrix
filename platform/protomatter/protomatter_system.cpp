#include <Arduino.h>
#include <system.h>
#include <renderer.h>
#include <game.h>
#include <input_wiichuck.h>
#include <input_encoder.h>

#define WAIT_FOR_SERIAL 0
#define PRINT_CONTROLLER_STATE

sys_time_t Time;

static Game game = {};

#define N_CONTROLLERS 4
Controller controllers[N_CONTROLLERS] = {};
const Controller *system_get_controller(uint8_t index)
{
  if (index < N_CONTROLLERS) return &controllers[index];
  return NULL;
}

uint32_t system_get_millis(void)
{
  return millis();
}

void system_delay(uint32_t ms)
{
  delay(ms);
}

#define SPEAKER_PIN A0
#define TONE_FREQUENCY 440 // Frequency of the tone in Hz (e.g., 440 Hz for A4 note)
#define TONE_DURATION 1000 // Duration of the tone in milliseconds
void playTronSound()
{
  uint16_t freq = 200;
  uint16_t maxFreq = 500;
  uint16_t stepSize = 1;

  while (freq < maxFreq)
  {
    for (uint16_t dutyCycle = 1; dutyCycle < 1023; dutyCycle <<= 1)
    {
      analogWrite(SPEAKER_PIN, dutyCycle);
      delayMicroseconds(freq);
      analogWrite(SPEAKER_PIN, 0);
      delayMicroseconds(freq);
    }
    freq += stepSize;
  }
}

void playTronBikeDrivingHum()
{
  uint16_t minFreq = 50;
  uint16_t maxFreq = 150;
  uint16_t stepSize = 1;

  for (uint16_t i = 0; i < 50; i++)
  {
    uint16_t freq = minFreq;
    while (freq < maxFreq)
    {
      analogWrite(SPEAKER_PIN, 128);
      delayMicroseconds(freq);
      analogWrite(SPEAKER_PIN, 0);
      delayMicroseconds(freq);
      freq += stepSize;
    }
    freq = maxFreq;
    while (freq > minFreq)
    {
      analogWrite(SPEAKER_PIN, 128);
      delayMicroseconds(freq);
      analogWrite(SPEAKER_PIN, 0);
      delayMicroseconds(freq);
      freq -= stepSize;
    }
  }
}

void playTronBikeTurn()
{
  uint16_t freq = 800;
  for (uint16_t i = 0; i < 20; i++)
  {
    analogWrite(SPEAKER_PIN, 128);
    delayMicroseconds(freq);
    analogWrite(SPEAKER_PIN, 0);
    delayMicroseconds(freq);
  }
}

void playTronBikeDestroyed()
{
  for (uint16_t freq = 1000; freq > 100; freq -= 20)
  {
    analogWrite(SPEAKER_PIN, 128);
    delayMicroseconds(freq);
    analogWrite(SPEAKER_PIN, 0);
    delayMicroseconds(freq);
  }
}

void playTronBikeCreated()
{
  for (uint16_t freq = 100; freq < 1000; freq += 20)
  {
    analogWrite(SPEAKER_PIN, 128);
    delayMicroseconds(freq);
    analogWrite(SPEAKER_PIN, 0);
    delayMicroseconds(freq);
  }
}

void playGameOverWin()
{
  for (uint16_t freq = 1000; freq < 2000; freq += 20)
  {
    analogWrite(SPEAKER_PIN, 128);
    delayMicroseconds(freq);
    analogWrite(SPEAKER_PIN, 0);
    delayMicroseconds(freq);
  }
}

void playGameOverLose()
{
  for (uint16_t freq = 2000; freq > 1000; freq -= 20)
  {
    analogWrite(SPEAKER_PIN, 128);
    delayMicroseconds(freq);
    analogWrite(SPEAKER_PIN, 0);
    delayMicroseconds(freq);
  }
}

void play_test_sounds()
{

  playTronBikeDrivingHum();
  delay(1000);
  playTronBikeTurn();
  delay(1000);
  playTronBikeDestroyed();
  delay(1000);
  playTronBikeCreated();
  delay(1000);
  playGameOverWin();
  delay(1000);
  playGameOverLose();
  delay(1000);
  playTronSound();
  delay(1000); // Wait for 1 second before repeating the sound
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)

  // while (!Serial) delay(100);
  Serial.printf("\n\nProtomatter System Setup\n");

  encoder_initAll();
  chuck_initAll();
  game = game_create();
  game.begin();

  renderer_init(game.get_render_context());

  pinMode(SPEAKER_PIN, OUTPUT);
}

void loop()
{
  // play_test_sounds();

  auto time = system_get_millis();
  Time.delta = time - Time.time;
  if (Time.delta < game.get_millis_per_frame()) return;
  Time.time = time;
  
  renderer_start_frame();
  encoder_updateAll();
  chuck_updateAll();

  for (size_t i = 0; i < N_CONTROLLERS; i++)
  {
    Controller *c = &controllers[i];
    c->active = false;

    if (encoder_isActive(i))
    {
      c->active = true;
      c->position = encoders[i].position;
    }

    if (chuck_isActive(i))
    {
      c->active = true;
      c->x = chucks[i].x;
      c->y = chucks[i].y;
    }

#ifdef PRINT_CONTROLLER_STATE
    Serial.printf("Controller %d: active: %d, x: %d, y: %d, position: %d\n", i, c->active, c->x, c->y, c->position);
#endif
  }

  game.loop();
  renderer_end_frame();
}
