/*
==================== Arduino Wiring Schematic ====================

[1] Stepper Motor Driver (TMC2225)
 - ENABLE_PIN : D2  → Driver ENABLE pin
 - STEP_PIN   : D3  → Driver STEP pin
 - DIR_PIN    : D4  → Driver DIR pin
 - Driver VCC / GND → Arduino 5V, GND
 - Driver VMOT / GND → External motor power (e.g., 12V, GND)
 - Driver outputs → 4 wires of stepper motor

[2] Load Cells with HX711 Amplifiers
 - Front Left  : DOUT1_PIN = D5,  CLK1_PIN = D6
 - Front Right : DOUT2_PIN = D7,  CLK2_PIN = D8
 - Back Left   : DOUT3_PIN = D9,  CLK3_PIN = D10
 - Back Right  : DOUT4_PIN = D11, CLK4_PIN = D12
 - Each HX711 VCC → Arduino 5V
 - Each HX711 GND → Arduino GND
 - Each HX711 connected to its load cell (E+, E−, A+, A− pins)

[3] OLED Display (SSD1306, I²C type)
 - SDA → A4 (Arduino I²C SDA)
 - SCL → A5 (Arduino I²C SCL)
 - VCC → Arduino 3.3V or 5V (depending on module)
 - GND → Arduino GND

[4] Start Button
 - BUTTON_PIN : D13
 - One side of button → D13
 - Other side of button → GND
 - Internal INPUT_PULLUP used, so no external resistor needed

[5] Power
 - Arduino powered by USB or external 5V supply
 - Ensure common ground between Arduino, stepper driver, HX711 modules, and OLED

===============================================================
*/




///// Get libraries

// For loadcell
#include <HX711.h>

// For OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


///// Set pin numbers

// Stepper motor pin
const int ENABLE_PIN = 2;
const int STEP_PIN = 3;
const int DIR_PIN = 4;

// Loadcell pin
// Front left
const int DOUT1_PIN = 5;
const int CLK1_PIN = 6;
// Front right
const int DOUT2_PIN = 7;
const int CLK2_PIN = 8;
// Back left
const int DOUT3_PIN = 9;
const int CLK3_PIN = 10;
// Back right
const int DOUT4_PIN = 11;
const int CLK4_PIN = 12;


///// Start button pin
const int BUTTON_PIN = 13;


///// Needed variables

// Loadcell calibration factors
const float CAL_FACTOR_1 = -12.0;
const float CAL_FACTOR_2 = -12.0;
const float CAL_FACTOR_3 = -12.0;
const float CAL_FACTOR_4 = -12.0;

// PID variables
const float ERROR_THRESHOLD = 1.0;
const float MIN_DELTA_TIME = 0.02;
const float MAX_DELTA_TIME = 1.0;

// Motor speed control variables
const long MIN_STEP_INTERVAL = 10;
const long MAX_STEP_INTERVAL = 500;
const long DEFAULT_STEP_INTERVAL = 100;

// Limit of motor
const float INTEGRAL_LIMIT = 1000.0;
const float OUTPUT_LIMIT = 50.0;

// Time variables
const unsigned long DISPLAY_UPDATE_INTERVAL = 200;
const unsigned long MAX_BALANCING_TIME = 300000;
const unsigned long DEBOUNCE_DELAY = 50;

// PID set variables
float pidKp = 3.0;
float pidKi = 0.01;
float pidKd = 0.5;
float pidError = 0.0;
float pidLastError = 0.0;
float pidIntegral = 0.0;
float pidOutput = 0.0;

// Millis timer variables
unsigned long previousPIDTime = 0;
unsigned long previousStepTime = 0;
unsigned long displayLastUpdate = 0;
unsigned long balancingStartTime = 0;

// Stepper speed controller
long stepInterval = DEFAULT_STEP_INTERVAL;

// OLED loadcell data show char variables
char displayBuffer1[10];
char displayBuffer2[10];
char displayBuffer3[10];
char displayBuffer4[10];

// System ready check variables
bool isSystemReady = false;


///// Make class models

// Set scales
HX711 scale1; // Front left
HX711 scale2; // Front right
HX711 scale3; // Back left
HX711 scale4; // Back right

// Set OLED display
Adafruit_SSD1306 display(128, 64, &Wire, -1);


///// OLED assosiated definitions

// Show text
void drawText(byte x, byte y, const char* text, byte size) {
  display.setCursor(x, y);
  display.setTextSize(size);
  display.print(text);
}

// Show state text
void displayMessage(const char* message) {
  display.clearDisplay();
  drawText(0, 0, message, 1);
  display.display();
}

// Set display
bool initializeDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    return false;
  }
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);
  display.display();
  return true;
}

// Show loadcell value
void updateDisplay(float val1, float val2, float val3, float val4) {
  unsigned long currentTime = millis();
  
  if (currentTime - displayLastUpdate < DISPLAY_UPDATE_INTERVAL) {
    return;
  }
  
  displayLastUpdate = currentTime;
  
  dtostrf(val1, 5, 1, displayBuffer1);
  dtostrf(val2, 5, 1, displayBuffer2);
  dtostrf(val3, 5, 1, displayBuffer3);
  dtostrf(val4, 5, 1, displayBuffer4);
  
  display.clearDisplay();
  drawText(0, 0, displayBuffer1, 1);
  drawText(64, 0, displayBuffer2, 1);
  drawText(0, 32, displayBuffer3, 1);
  drawText(64, 32, displayBuffer4, 1);
  display.display();
}


///// Steppermotor related definitions(TMC2225 motor driver)

// Set steppermotor
void initializeStepper() {
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
}

// Turn steppermotor
void executeStepper() {
  unsigned long currentTime = millis();
  
  if (currentTime - previousStepTime >= stepInterval) {
    previousStepTime = currentTime;
    
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(STEP_PIN, LOW);
  }
}


///// Loadcell related definitions(HX711)

// Set loadcell
bool initializeLoadCells() {
  scale1.begin(DOUT1_PIN, CLK1_PIN);
  scale2.begin(DOUT2_PIN, CLK2_PIN);
  scale3.begin(DOUT3_PIN, CLK3_PIN);
  scale4.begin(DOUT4_PIN, CLK4_PIN);
  
  if (!scale1.is_ready() || !scale2.is_ready() || 
      !scale3.is_ready() || !scale4.is_ready()) {
    return false;
  }
  
  scale1.set_scale(CAL_FACTOR_1);
  scale1.tare();
  scale2.set_scale(CAL_FACTOR_2);
  scale2.tare();
  scale3.set_scale(CAL_FACTOR_3);
  scale3.tare();
  scale4.set_scale(CAL_FACTOR_4);
  scale4.tare();
  
  return true;
}

// Get loadcell data
bool readLoadCells(float& val1, float& val2, float& val3, float& val4) {
  if (!scale1.is_ready() || !scale2.is_ready() || 
      !scale3.is_ready() || !scale4.is_ready()) {
    return false;
  }
  
  val1 = scale1.get_units(1);
  val2 = scale2.get_units(1);
  val3 = scale3.get_units(1);
  val4 = scale4.get_units(1);
  
  return true;
}


///// PID related definitions

// Set PID
void resetPID() {
  pidLastError = 0.0;
  pidIntegral = 0.0;
  pidError = 0.0;
  pidOutput = 0.0;
  previousPIDTime = millis();
}

// Calculate PID
void calculatePID(float error, float deltaTime) {
  pidIntegral += error * deltaTime;
  pidIntegral = constrain(pidIntegral, -INTEGRAL_LIMIT, INTEGRAL_LIMIT);
  
  float derivative = (error - pidLastError) / deltaTime;
  pidOutput = pidKp * error + pidKi * pidIntegral + pidKd * derivative;
  pidOutput = constrain(pidOutput, -OUTPUT_LIMIT, OUTPUT_LIMIT);
  
  long calculatedInterval = (long)(1000.0 / (abs(pidOutput) + 1.0));
  stepInterval = constrain(calculatedInterval, MIN_STEP_INTERVAL, MAX_STEP_INTERVAL);
  
  if (pidOutput > 0) {
    digitalWrite(DIR_PIN, HIGH);
  } else {
    digitalWrite(DIR_PIN, LOW);
  }
  
  pidLastError = error;
}


///// Button related definitions

// Check button
bool isButtonPressed() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(DEBOUNCE_DELAY);
    if (digitalRead(BUTTON_PIN) == LOW) {
      return true;
    }
  }
  return false;
}


///// Balancing related definitions

// Check balancing timeout
bool isBalancingTimeout() {
  return (millis() - balancingStartTime) > MAX_BALANCING_TIME;
}

// Go to balancing point
void runBalancingCycle() {
  displayMessage("bt Clicked!");
  delay(1000);
  
  resetPID();
  balancingStartTime = millis();
  
  while (true) {
    if (isButtonPressed()) {
      displayMessage("Emergency Stop!");
      delay(2000);
      return;
    }
    
    if (isBalancingTimeout()) {
      displayMessage("Timeout!");
      delay(2000);
      return;
    }
    
    unsigned long currentTime = millis();
    float deltaTime = (float)(currentTime - previousPIDTime) / 1000.0;
    
    if (deltaTime < MIN_DELTA_TIME) {
      delay(5);
      continue;
    }
    
    if (deltaTime > MAX_DELTA_TIME) {
      previousPIDTime = currentTime;
      continue;
    }
    
    previousPIDTime = currentTime;
    
    float scale1Val, scale2Val, scale3Val, scale4Val;
    
    if (!readLoadCells(scale1Val, scale2Val, scale3Val, scale4Val)) {
      displayMessage("Sensor Error!");
      delay(2000);
      return;
    }
    
    float scaleFront = scale1Val + scale2Val;
    float scaleBack = scale3Val + scale4Val;
    pidError = scaleFront - scaleBack;
    
    updateDisplay(scale1Val, scale2Val, scale3Val, scale4Val);
    
    if (abs(pidError) <= ERROR_THRESHOLD) {
      displayMessage("Done!!");
      delay(2000);
      return;
    }
    
    calculatePID(pidError, deltaTime);
    executeStepper();
  }
}

void setup() {
  // Set button pinMode
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Set display and check if it is OK
  if (!initializeDisplay()) {
    while (true) {
      delay(1000);
    }
  }
  displayMessage("display set end!");
  delay(2000);
  
  // Set stepper
  display.clearDisplay();
  initializeStepper();
  displayMessage("stepper set end!");
  delay(2000);
  
  // Set loadcell and check if it is OK
  display.clearDisplay();
  if (!initializeLoadCells()) {
    displayMessage("loadcell fail!");
    while (true) {
      delay(1000);
    }
  }
  displayMessage("loadcell set end!");
  delay(2000);
  display.clearDisplay();
  display.display();
  
  // Make system ready state true
  isSystemReady = true;
}

void loop() {
  // Check system error
  if (!isSystemReady) {
    displayMessage("System Error!");
    delay(1000);
    return;
  }
  
  // Before button is clicked
  displayMessage("Ready....");
  
  // Go to cog if button is pressed
  if (isButtonPressed()) {
    runBalancingCycle();
  }
}
