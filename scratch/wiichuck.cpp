#include <Wire.h>
#include <Wiichuck.h>

#define PCAADDR 0x70

void pcaselect(uint8_t i) {
  if (i > 3) return;

  Wire.beginTransmission(PCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

// Wiichuck nunchuck[4];
Accessory chucks[4];

void setup() {
  Wire.begin();
	Serial.begin(115200);
  
  for (int i = 0; i < 4; i++) {
    pcaselect(i);
    auto nunchuck = chucks[i];
    nunchuck.begin();
    if (nunchuck.type == Unknown) {
		  nunchuck.type = NUNCHUCK;
    }
  }
}

void loop() {
  for (int i = 0; i < 4; i++) {
    pcaselect(i);
    auto chuck = chucks[i];

    chuck.readData();    // Read inputs and update maps
    Serial.print("Nunchuck "); 
    Serial.print(i); 
    Serial.print(":"); 
    Serial.println(chuck.getJoyX());
    
    Serial.print("X: "); Serial.print(chuck.getAccelX());
    Serial.print(" \tY: "); Serial.print(chuck.getAccelY()); 
    Serial.print(" \tZ: "); Serial.println(chuck.getAccelZ()); 

    Serial.print("Joy: ("); 
    Serial.print(chuck.getJoyX());
    Serial.print(", "); 
    Serial.print(chuck.getJoyY());
    Serial.println(")");

    Serial.print("Button: "); 
    if (chuck.getButtonZ()) Serial.print(" Z "); 
    if (chuck.getButtonC()) Serial.print(" C "); 

    Serial.println();
  }
  
  delay(100);
}