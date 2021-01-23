#include <Wire.h>
#include <SPI.h>
#include <BH1750.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Metro.h>

/*
 * BH1750
 * https://github.com/claws/BH1750
 * VCC -> 3V3 or 5V
 * GND -> GND
 * SCL -> SCL (A5)
 * SDA -> SDA (A4)
 * ADD -> GND
 * Low Resolution Mode    - (generic range: 0.0 up to 54612.5 lux)  - (4 lx precision, 16ms measurement time)
 * High Resolution Mode   - (generic range: 0.0 up to 54612.5 lux)  - (1 lx precision, 120ms measurement time)
 * High Resolution Mode 2 - (generic range: 0.0 up to 27306.25 lux) - (0.5 lx precision, 120ms measurement time)
 */
BH1750 lightMeter;

/* 
 * BME280 
 * 0 m   100 %   1013,25 hPa
 * 100 m  98,7 % 1000,3 hPa
 * 200 m  97,5 %  987,6 hPa
 * 300 m  96,2 %  975,0 hPa
 */
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;

/*
 * LM358B
 * CS   is pin 10
 * SCK  is pin 13
 * MISO is pin 12
 * MOSI is pin 11
 */
#define DAC_CS 10
#define DAC_A_ON  0b00110000
#define DAC_A_OFF 0b00100000
#define DAC_B_ON  0b10110000
#define DAC_B_OFF 0b10100000
// Calibration data obtained by running the calibration code
const int DAC_A_0V  = 0;    //  0,682V
const int DAC_A_10V = 3900; // 10,68V
const int DAC_B_0V  = 0;    //  0,685V
const int DAC_B_10V = 3910; // 10,69V
// Data min and max range
const int data_min_range = 0;
const int data_max_range = 100;
int dacInt = 0;

// LED
const int ledPin = 9;
int ledState = LOW;

// Timer
Metro dacMetro = Metro(300);
Metro ledMetro = Metro(500);
Metro sensMetro = Metro(2000);

void setup() {

  // initialize serial:
  Serial.begin(9600);
  Serial.println("Lux & Air Sensor Test");

  // BH1750
  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();
  lightMeter.begin();

  // BME280
  unsigned status;
  status = bme.begin();  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
    while (1);
  }

  // Initialize SPI & DAC
  pinMode(DAC_CS, OUTPUT);
  digitalWrite(DAC_CS, 1);
  SPI.begin();
  set_DAC_off('A');
  set_DAC_off('B');

  // LED
  pinMode(ledPin, OUTPUT);

  Serial.println("-- Default Test --");
}

void loop() {

  if (dacMetro.check() == 1) {
    int dacA, dacB;
    if (dacInt <= 100) {
      dacA = dacInt;
      dacB = 100 - dacInt;
      dacInt += 1;
    } else if (dacInt > 100) {
      dacA = 200 - dacInt;
      dacB = dacInt - 100;
      if (dacInt < 200) {
        dacInt += 1;
      } else {
        dacInt = 1;
      }
    }
    SendTo0_10V('A', dacA);
    SendTo0_10V('B', dacB);
  }

  if (ledMetro.check() == 1) {
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);
  }

  if (sensMetro.check() == 1) {
    printBH1750Values();
    printBME280Values();
    Serial.println("------------------------------");
  }

}

void printBH1750Values() {
  float lux = lightMeter.readLightLevel();
  Serial.print("Light            = ");
  Serial.print(lux);
  Serial.println(" Lux");
}

void printBME280Values() {
  Serial.print("Temperature      = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressure         = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity         = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
}

/*
  DAC works on SPI
  We must send 16 bits
  byte1 is [WR, BUF, /GA, /SHDN, data11, data10, data9, data8]
  byte2 is [data7, data6, data5, data4, data3, data2, data1, data0]
  Write code
  WR    0 - write to DAC register
        1 - Ignore this command
  VREF Input Buffer Control bit
  BUF   0 - Unbuffered
        1 - Buffered
  Output Gain Selection bit
  /GA   0 - 1x (VOUT = VREF * D/4096)
        1 - 0 = 2x (VOUT = 2 * VREF * D/4096)
  Output Shutdown Control bit
  /SHDN 0 - Shutdown the device. Analog output is not available.
        1 - Active mode operation. VOUT is available
  
  WR has to be 0 to write to DAC registers
  BUF is set to 0 (unbuffered)
  GAIN MUST BE SET TO 1. We can't output more than the Vcc!!!
  SHDN also set to 1 to have DAC active.
  
  00110000 = 0x30 <- DAC_A on
  00100000 = 0x20 <- DAC_A off
  10110000 = 0xB0 <- DAC_B on
  10100000 = 0xA0 <- DAC_B off
*/
void set_DAC(char dac, int set_value) {
  byte first_byte;
  byte second_byte;
  first_byte = (set_value >> 8) & 0x0F;
  if (dac == 'A') {
    first_byte = first_byte | DAC_A_ON;
  } else {
    first_byte = first_byte | DAC_B_ON;
  }
  second_byte = set_value & 0xFF;
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(DAC_CS, 0);
  SPI.transfer(first_byte);
  SPI.transfer(second_byte);
  digitalWrite(DAC_CS, 1);
  SPI.endTransaction();
}

void set_DAC_off(char dac) {
  byte first_byte;
  byte second_byte;
  if (dac == 'A') {
    first_byte = DAC_A_OFF;
  } else {
    first_byte = DAC_B_OFF;
  }
  second_byte = 0x00;
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(DAC_CS, 0);
  SPI.transfer(first_byte);
  SPI.transfer(second_byte);
  digitalWrite(DAC_CS, 1);
  SPI.endTransaction();
}

void SendTo0_10V(char dac, unsigned int transmitted_Value) {
  // Map the data to be sent into DAC values
  int temp = 0;
  if (dac == 'A') {
    temp = map(transmitted_Value, data_min_range, data_max_range, DAC_A_0V, DAC_A_10V);
  } else {
    temp = map(transmitted_Value, data_min_range, data_max_range, DAC_B_0V, DAC_B_10V);
  }
  set_DAC(dac, temp);
}
