/*
Weight scale by Konstantinos Anastasakis //https://github.com/kon-anast/kan-weight_scale
My blog https://kostislab.blogspot.gr/

See README.md for wiring, building, and calibration instructions.

*/

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  //https://bitbucket.org/fmalpartida/new-liquidcrystal/src
#include "HX711.h"  //https://github.com/bogde/HX711
#include "LowPower.h" //http://www.rocketscream.com/blog/2011/07/04/lightweight-low-power-arduino-library/
// #include <Sodaq_DS3231.h>  //RTC Library https://github.com/SodaqMoja/Sodaq_DS3231

// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // For i2c adapter v1
// LiquidCrystal_I2C lcd(0x20, 4, 5, 6, 0, 1, 2, 3, 7, NEGATIVE);  // For i2c adapter v2

const uint8_t dataPin = 7;
const uint8_t clockPin = 6;
HX711 scale(dataPin, clockPin);

const uint8_t potPin = A2;

float calibration_factor = 0;

// HX711 readings are 24-bit values, so they do not fit in an AVR int.
long new_weight = 0;
long old_weight = 0;
long delta = 0;

const uint8_t calibButton = 10;
const uint8_t zeroButton = 11;
const uint8_t memoryButton = 12;

// Voltage Reference pin
const uint8_t voltRefPin = A0;
const float voltMult = 2.35; // Measured ADC reference voltage.
// Voltage divider resistors
const float resistor1 = 10000.0; // R1 resistor connected to GND and A0.
const float resistor2 = 7500.0; // R2 connected to Vcc and A0.

// Sleep
  // const int interval = 32000; // Interval is how long we wait until it goes to sleep
  // unsigned long previousMillis=0; // Tracks the time since last event fired
  // unsigned long currentMillis;

//----Functions----//

void clearLine(uint8_t row) {
  lcd.setCursor(0, row);
  lcd.print("                ");
}

void sleep_code() {
  scale.power_down(); // put the ADC in sleep mode
  lcd.noDisplay();
  lcd.noBacklight(); // turn off backlight

  while (digitalRead(calibButton) == HIGH && digitalRead(zeroButton) == HIGH &&
         digitalRead(memoryButton) == HIGH) {
    LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
  }

  scale.power_up();
  lcd.display();
  lcd.backlight();
}

void calibration_code() {
  lcd.setCursor(0, 0);
  lcd.print("Turn the potenti");
  lcd.print(" ");
  lcd.print(" ");
  delay(1000);

  while (digitalRead(calibButton) == LOW) {
    calibration_factor = max(1, analogRead(potPin));
    scale.set_scale(calibration_factor);
    lcd.setCursor(0, 0);
    lcd.print("Grams: ");
    lcd.print(scale.get_units(5), 1);
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print("CalFactor:");
    lcd.print(calibration_factor);
    lcd.print(" ");
    lcd.print(" ");
    lcd.print(" ");
  }

  clearLine(1);
}

void zero_code() {
  lcd.setCursor(0, 1);
  lcd.print("Zeroing");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  scale.tare();
  delay(500);
  clearLine(1);
}

void memory_code() {
  long memoryvalue = scale.read_average();
  delay(10);
  lcd.setCursor(0, 1);
  lcd.print("A=");
  lcd.print(memoryvalue);
  lcd.print(" ");
  lcd.print("Sum=");
  lcd.print(memoryvalue);
  lcd.print(" ");
  lcd.print(" ");
}
// End function

void vref_code() {

  // Convert resistor values to division value.
  //  voltage divider equation: R2 / (R1 + R2)
  const float denominator = resistor2 / (resistor1 + resistor2);

  // Obtain RAW voltage data.
  float voltage = analogRead(voltRefPin);
  // Convert to actual voltage using the measured ADC reference.
  voltage = (voltage / 1024) * voltMult;
  //Convert to voltage before divider
  //  Divide by divider = multiply
  //  Divide by 1/5 = multiply by 5
  voltage = voltage / denominator;

  //Output to serial & LCD
  //  Serial.print("Volts: ");
  //  Serial.println(voltage);
  //  delay(500);  //Delay to make serial out readable

	lcd.setCursor(0, 1);
  lcd.print(voltage);
	lcd.print("v ");

} // void vref_code close

void setup() {
  // Serial.begin(9600);

  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  // pinMode(6, OUTPUT);
  // pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  // pinMode(10, INPUT_PULLUP);
  // pinMode(11, INPUT_PULLUP);
  // pinMode(12, INPUT_PULLUP);
  // pinMode(13, INPUT_PULLUP); //this powers the led that is connected on pin13
  pinMode(14, INPUT_PULLUP); //A0 *Analog pins
  pinMode(15, INPUT_PULLUP); //A1
  // pinMode(16, INPUT_PULLUP); //A2
  pinMode(17, INPUT_PULLUP); //A3
  pinMode(18, INPUT_PULLUP); //A4
  pinMode(19, INPUT_PULLUP); //A5
  pinMode(20, INPUT_PULLUP); //A6
  pinMode(21, INPUT_PULLUP); //A7

  pinMode(calibButton, INPUT_PULLUP);
  pinMode(zeroButton, INPUT_PULLUP);
  pinMode(memoryButton, INPUT_PULLUP);
  // Keep in mind the pull-up means the pushbutton's logic is inverted. It goes
  // HIGH when it's open, and LOW when it's pressed.

  // set up for batt voltage measurement
  // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc external reference
  // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG)         -Selects channel 14, bandgap voltage, to measure
  // ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
  // delay(50);  // Let mux settle a little to get a more stable A/D conversion

	// vRef.begin();	// Start Voltage Reference

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.display();
  lcd.backlight();
  // lcd.clear();  //Clear the lcd

  scale.power_up();
  delay(10);
  scale.set_scale();
  delay(10);
  scale.tare();  //Reset the scale to 0

// Serial print for debaging
//   Serial.println("HX711 calibration sketch");
//   Serial.println("Remove all weight from scale");
//   Serial.println("After readings begin, place known weight on scale");
//   Serial.println("Turn the potentiometer in inverse proportion of the measurements");
}

void loop() {
  new_weight = scale.read_average();
  delta = new_weight - old_weight;
  calibration_factor = max(1, analogRead(potPin)); // Avoid division by zero.
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  lcd.setCursor(0, 0);
  lcd.print("Grams: ");
  lcd.print(scale.get_units(5), 1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  lcd.print(" ");
  // lcd.setCursor(0, 1);

	vref_code();

  if (digitalRead(calibButton) == LOW) {calibration_code();}
  if (digitalRead(zeroButton) == LOW) {zero_code();}
  if (digitalRead(memoryButton) == LOW) {memory_code();}

// Send the scale to sleep based on how much time has passed
  // Get snapshot of time
  // unsigned long currentMillis = millis();
  // if ((unsigned long)(currentMillis - previousMillis) >= interval) {
  //    sleep_code(); // Go for sleep if it is more then interval, will be chaged if use Soft power system
  //    // Use the snapshot to set track time until next event
  //    previousMillis = currentMillis;
  // }
// Send the scale to sleep be counting the loops,
  // if (delta < 10000) {count++;} else {count = 0;}
  // if (count > maxnum) {sleep_code();}

// Serial print for debaging
//   Serial.print("calibration_factor: ");
//   Serial.println(calibration_factor);
//   Serial.print("Reading: ");
//   delay(100);
//   Serial.print(scale.get_units(5), 1);
//   Serial.println(" grams");
//   long zero_factor = scale.read_average(); //Get a baseline reading
//   Serial.print("Zero factor: "); //This can be used to remove the need to
//    //tare the scale. Useful in permanent scale projects.
//   Serial.println(zero_factor);
//   Serial.print("delta");
//   Serial.println(delta);
//   Serial.print("new_weight");
//   Serial.println(new_weight);
//   Serial.print("old_weight");
//   Serial.println(old_weight);
//   Serial.print("count");
//   Serial.println(count);

  old_weight = new_weight; // Update the old readings
}
