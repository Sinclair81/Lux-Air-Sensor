# Lux & Air Sensor  
  
[![donate](https://img.shields.io/badge/donate-PayPal-blue.svg)](https://www.paypal.me/Sinclair81)  
  
<!-- markdownlint-disable MD033 -->  
<img src="https://github.com/Sinclair81/Lux-Air-Sensor/blob/main/Lux-Air-Sensor_Top.png" align="right" alt="Lux-Sensor_Top" height="233" width="350">  
<!-- markdownlint-enable MD033 -->  
  
A [Arduino](https://www.arduino.cc) [Pro Mini](https://store.arduino.cc/arduino-pro-mini) like PCB  
with Lux, Humidity, Air Pressure and Temperature Sensor  
and 2x 0-10V Output.  
  
U1 - ATmega328P (3.3V, 8MHz)  
U2 - MCP4922 (2x DAC)  
U3 - LM358B (2x OP AMP)  
U4 - HB1750 (Lux Sensor)  
U5 - BME280 (Humidity, Air Pressure and Temperature Sensor)  
U6 - L78M12 (12V)  
U7 - MIC5205 (3.3V)  
  
Name                 | Pin                   | Function  
-------------------- | --------------------- | ---------------------  
RXD                  | D0                    |  
TXD                  | D1                    |  
Jumper 1             | D2                    |  0   - 600 Lux  
Jumper 2             | D3                    |  0   - 3000 Lux  
Jumper 3             | D4                    |  600 - 10000 Lux  
Jumper 4             | D5                    |  0   - 100% rH  
Jumper 5             | D6                    |  300 - 1100 hPa  
Jumper 6             | D7                    | -25  - 55 °C  
Jumper 7             | D8                    |  0   - 50 °C  
LED 1                | D9                    |  
MCP4922 #CS          | D10                   |  
MCP4922 SDI          | D11                   |  
MCP4922 SCK          | D13                   |  
Pin A0               | A0                    |  
Pin A1               | A1                    |  
Pin A2               | A2                    |  
Pin A3               | A3                    |  
HB1750 / BME280 SDA  | A4                    |  
HB1750 / BME280 SCL  | A5                    |  
