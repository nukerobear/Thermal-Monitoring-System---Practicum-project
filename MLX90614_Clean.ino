/*
LCD Code: From Arduino Library
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the I2C and LCD library code:
#include <i2cmaster.h>
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int analogPin = 3;   // Define input pin for CT 
int current_raw = 0; // Raw count value of input voltage
int current = 0;     // Scaled actual current value 

void setup(){
	Serial.begin(9600);
	Serial.println("Setup...");
	
	i2c_init(); //Initialise the i2c bus
	PORTC = (1 << PORTC4) | (1 << PORTC5);//enable pullups

// set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Current: ");
}

void loop(){
    // Code for Infrared sensor
    int dev = 0x5A<<1;
    int data_low = 0;
    int data_high = 0;
    int pec = 0;
    
    i2c_start_wait(dev+I2C_WRITE);
    i2c_write(0x07);
    
    // read
    i2c_rep_start(dev+I2C_READ);
    data_low = i2c_readAck(); //Read 1 byte and then send ack
    data_high = i2c_readAck(); //Read 1 byte and then send ack
    pec = i2c_readNak();
    i2c_stop();
    
    //This converts high and low bytes together and processes temperature, 
    //MSB is a error bit and is ignored for temps
    double tempFactor = 0.02; // 0.02 degrees per LSB (measurement resolution of the MLX90614)
    double tempData = 0x0000; // zero out the data
    int frac; // data past the decimal point
    
    // This masks off the error bit of the high byte, then moves it left 8 bits and adds the low byte.
    tempData = (double)(((data_high & 0x007F) << 8) + data_low);
    tempData = (tempData * tempFactor)-0.01;
    
    // Read voltage reading for current monitoring 
    current_raw = analogRead(3);
    current = 33.3 + current_raw/3.95; // Actual current value scaling
    
    float celcius = tempData - 273.15;
    float fahrenheit = (celcius*1.8) + 32;
    
  // Printing to LCD
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(fahrenheit,2);
  lcd.setCursor(12, 1);
  lcd.print((char)223);  // Degree Symbol
  lcd.print("F");
  lcd.setCursor(10, 0);
  
  // System displays current values only above 34A
  if (current < 34)
  {
     lcd.print("< 34");  
  }
  else
  {
  lcd.print(current);
  }
  lcd.print("A  ");
    delay(1000); // wait a second before printing again
}
