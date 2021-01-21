/*
 * https://github.com/claws/BH1750
 * 
 * VCC -> 3V3 or 5V
 * GND -> GND
 * SCL -> SCL (A5)
 * SDA -> SDA (A4)
 * ADD -> GND
 * 
 * Low Resolution Mode    - (generic range: 0.0 up to 54612.5 lux)  - (4 lx precision, 16ms measurement time)
 * High Resolution Mode   - (generic range: 0.0 up to 54612.5 lux)  - (1 lx precision, 120ms measurement time)
 * High Resolution Mode 2 - (generic range: 0.0 up to 27306.25 lux) - (0.5 lx precision, 120ms measurement time)
 * 
 */

#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

void setup(){

  Serial.begin(9600);

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();

  lightMeter.begin();
  Serial.println(F("BH1750 Test"));

}

void loop() {

  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(1000);

}
