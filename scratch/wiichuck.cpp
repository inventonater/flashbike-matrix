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
    auto nunchuck = chucks[i];

    nunchuck.readData();    // Read inputs and update maps
    Serial.print("Nunchuck "); 
    Serial.print(i); 
    Serial.print(":"); 
    Serial.println(nunchuck.getJoyX());
    
    continue;
    Serial.print("X: "); Serial.print(nunchuck.getAccelX());
    Serial.print(" \tY: "); Serial.print(nunchuck.getAccelY()); 
    Serial.print(" \tZ: "); Serial.println(nunchuck.getAccelZ()); 

    Serial.print("Joy: ("); 
    Serial.print(nunchuck.getJoyX());
    Serial.print(", "); 
    Serial.print(nunchuck.getJoyY());
    Serial.println(")");

    Serial.print("Button: "); 
    if (nunchuck.getButtonZ()) Serial.print(" Z "); 
    if (nunchuck.getButtonC()) Serial.print(" C "); 

    Serial.println();
  }
  
  delay(100);
}