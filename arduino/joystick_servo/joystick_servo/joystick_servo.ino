/******************************************************************************
Screwball Arduino-Joystick-Servo Control 

Using Qwiic Joystiic Example 1 - Basic Readings
Nick Poole @ SparkX
Feb 6, 2018
https://github.com/sparkfunX/Qwiic_Joystiic

This sketch communicates with the Qwiic Joystiic breakout using I2C and 
reports the horizontal, vertical and button values at a rate of 1Hz using
the serial port.

If a connection is succesfully made to the Joystiic on startup, the device
will also return its currently configured I2C Slave Address. For new units, 
this address will be 0x20, any other value on a new unit may be symptomatic 
of communication error.

Connect SCL to A5, SDA to A4

******************************************************************************/

#include <Wire.h>
#include <Servo.h> 
 
Servo horizontalservo;  // create servo object to control a servo 
Servo verticalservo;  // create servo object to control a servo 


byte DEV_ADDR = 0x20;
int reading = 0;
int horizontalval = 0;
int verticalval = 0;
boolean buttonval = 0;

  
int zero_horizontal;
int zero_vertical;
boolean zero_button;

void setup() {

  Wire.begin();        // join i2c bus
  Serial.begin(14400);  // start serial for output

  // Print the address currently in EEPROM on the Joystiic
  Serial.print("Slave Address in EEPROM: 0x");
  Serial.println(getAddr(), HEX);
  
  horizontalservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  verticalservo.attach(8);  // attaches the servo on pin 9 to the servo object 
  
  delay(100);
  
  while ((zero_horizontal > 1023) | (zero_horizontal < 0) | (zero_vertical > 1023) | (zero_vertical < 0) | (zero_button > 1) | (zero_button < 0)){
    Serial.println("Start Values Are Errored");
    zero_horizontal = getHorizontal();
    zero_vertical = getVertical();
    zero_button = getButton();
  }
  

}

void loop() {
  
  horizontalval = getHorizontal();
  verticalval = getVertical();
  buttonval = getButton();
  
  if ((buttonval == 1) | (buttonval == 0)){
  
  if (abs(horizontalval - zero_horizontal) < 10){
    horizontalval = zero_horizontal;
  }
  if (abs(verticalval - zero_vertical) < 10){
    verticalval = zero_vertical;
  }
  
  if ((horizontalval == 496) && (verticalval == 486)){
    Serial.println("zero");
    ;
  }
  else{
  Serial.print("H: ");
  Serial.println(horizontalval);
  Serial.print("V: ");
  Serial.println(verticalval);
  Serial.print("B: ");
  Serial.println(buttonval);
  Serial.println();
  }
  
  

  
  if ((horizontalval < 1023) && (horizontalval > 0)){
    horizontalservo.write(map(horizontalval, 0, 1023, 120, 80));
  }
  if ((verticalval < 1023) && (verticalval > 0)){
    verticalservo.write(map(verticalval, 0, 1023, 67, 100));
  }
 delay(40);
  }
  else{
    Serial.println("buttonval is error");
  Serial.print("H: ");
  Serial.println(horizontalval);
  Serial.print("V: ");
  Serial.println(verticalval);
  Serial.print("B: ");
  Serial.println(buttonval);
  Serial.println();
  }
  

}


// getHorizontal() returns an int between 0 and 1023 
// representing the Joystiic's horizontal position
// (axis indicated by silkscreen on the board)
// centered roughly on 512

int getHorizontal() {

  Wire.beginTransmission(DEV_ADDR);
  Wire.write(0x00);
  Wire.endTransmission(false);
  Wire.requestFrom(DEV_ADDR, 2);

  while (Wire.available()) {
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    reading = (uint16_t)msb << 8 | lsb;
  }

  Wire.endTransmission();
  return reading;

}

// getVertical() returns an int between 0 and 1023 
// representing the Joystiic's vertical position
// (axis indicated by silkscreen on the board)
// centered roughly on 512

int getVertical() {

  Wire.beginTransmission(DEV_ADDR);
  Wire.write(0x02);
  Wire.endTransmission(false);
  Wire.requestFrom(DEV_ADDR, 2);

  while (Wire.available()) {
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    reading = (uint16_t)msb << 8 | lsb;
  }

  Wire.endTransmission();
  return reading;

}

// getButton() returns a byte indicating the 
// position of the button where a 0b1 is not
// pressed and 0b0 is pressed

byte getButton() {

  byte btnbyte = 0b00000000;

  Wire.beginTransmission(DEV_ADDR);
  Wire.write(0x04);
  Wire.endTransmission(false);
  Wire.requestFrom(DEV_ADDR, 1);    // request 1 byte

  while (Wire.available()) {
    btnbyte = Wire.read();
  }

  Wire.endTransmission();
  return btnbyte;

}

// getAddr() returns a byte indicating the 
// 7-bit Slave Address that the Joystiic 
// currently has stored in EEPROM. Because
// address changes do not take effect until
// power reset, the value returned from 
// getAddr() may not match the address that 
// the device is currently using. As a 
// result, this call can be used to confirm
// that a setAddr() call was successful.

byte getAddr() {

  byte addr = 0x00;

  Wire.beginTransmission(DEV_ADDR);
  Wire.write(0x06);
  Wire.endTransmission(false);
  Wire.requestFrom(DEV_ADDR, 1);    // request 1 byte

  while (Wire.available()) {
    addr = Wire.read();
  }

  Wire.endTransmission();
  return addr;

}

// setAddr() takes a 7-bit I2C Slave Address 
// and passes it to the Joystiic. The Joystiic
// will store this address in EEPROM and use
// it on power reset. If a reserved address is
// passed to this function, it will send it to
// the Joystiic, but the Joystiic will not set
// store it in EEPROM. 
// getAddr() can be used to confirm
// that a setAddr() call was successful.

void setAddr(byte addr) {

  Wire.beginTransmission(DEV_ADDR);
  Wire.write(0x05);
  Wire.write(addr);
  Wire.endTransmission();

}


