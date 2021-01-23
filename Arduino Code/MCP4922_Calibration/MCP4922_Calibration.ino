
#include <SPI.h>

// LM358B
// CS   is pin 10
// SCK  is pin 13
// MISO is pin 12
// MOSI is pin 11
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
const int data_max_range = 600;

String inputString  = "";     // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete


void setup() {

  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  // Initialize SPI
  pinMode(DAC_CS, OUTPUT);
  digitalWrite(DAC_CS, 1);
  SPI.begin();

  set_DAC_off('A');
  set_DAC_off('B');
}

void loop() {

   // print the string when a newline arrives:
  if (stringComplete) {
    int x = inputString.toInt();
    if (x < 0) {
      x = 0;
      inputString = ">>> 0";
    }
    if (x > 4096) {
      x = 4096;
      inputString = ">>> 4096";
    }
    
    // set_DAC('A', x);
    // set_DAC('B', x);

    SendTo0_10V('A', x);
    SendTo0_10V('B', x);
    
    Serial.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
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
  if (transmitted_Value < data_min_range) {
    transmitted_Value = data_min_range;
  }
  if (transmitted_Value > data_max_range) {
    transmitted_Value = data_max_range;
  }
  int temp = 0;
  if (dac == 'A') {
    temp = map(transmitted_Value, data_min_range, data_max_range, DAC_A_0V, DAC_A_10V);
  } else {
    temp = map(transmitted_Value, data_min_range, data_max_range, DAC_B_0V, DAC_B_10V);
  }
  set_DAC(dac, temp);
}