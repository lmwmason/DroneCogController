///// oled -> SDA-A4, SCL-A5

#include <HX711.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// stepper pins
const int enable = 2;
const int step = 3;
const int dir = 4;

/// hx711 pins
const int dout1 = 5;
const int clk1 = 6;
const int dout2 = 7;
const int clk2 = 8;
const int dout3 = 9;
const int clk3 = 10;
const int dout4 = 11;
const int clk4 = 12;

/// bt pin
const int bt = 13;

// set your loadcell
HX711 scale1;
HX711 scale2;
HX711 scale3;
HX711 scale4;

// set calibration factor of each loadcell
const float cal_fac1 = -12;
const float cal_fac2 = -12;
const float cal_fac3 = -12;
const float cal_fac4 = -12;

// set oled
Adafruit_SSD1306 display(128, 64, &Wire, -1);

unsigned long previousStepTime = 0;
const long stepInterval = 20;

void draw_text(byte x_pos, byte y_pos, char *text, byte text_size) {
  display.setCursor(x_pos, y_pos);
  display.setTextSize(text_size);
  display.print(text);
}

void setup() {
  Serial.begin(9600);

  pinMode(bt, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE, BLACK);
  display.print("display set end!");
  display.display();
  delay(2000);

  display.clearDisplay();
  pinMode(enable, OUTPUT);
  pinMode(step, OUTPUT);
  pinMode(dir, OUTPUT);
  digitalWrite(enable, LOW);
  digitalWrite(step, LOW);
  digitalWrite(dir, LOW);
  display.print("stepper set end!");
  display.display();
  delay(2000);

  display.clearDisplay();
  scale1.begin(dout1, clk1);
  scale2.begin(dout2, clk2);
  scale3.begin(dout3, clk3);
  scale4.begin(dout4, clk4);
  scale1.set_scale(cal_fac1);
  scale1.tare();
  scale2.set_scale(cal_fac2);
  scale2.tare();
  scale3.set_scale(cal_fac3);
  scale3.tare();
  scale4.set_scale(cal_fac4);
  scale4.tare();
  display.print("loadcell set end!");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
}

void loop() {
  display.clearDisplay();
  display.print("Ready....");
  if(analogRead(bt)==0)
  {
    display.clearDisplay();
    display.print("bt Clicked!");
    display.display();
    delay(1000);
    while(1)
    {
      unsigned long currentMillis = millis();
      display.clearDisplay();
      float scale1_val = scale1.get_units(5);
      float scale2_val = scale2.get_units(5);
      float scale3_val = scale3.get_units(5);
      float scale4_val = scale4.get_units(5);
      
      char buf1[10], buf2[10], buf3[10], buf4[10]; 
      dtostrf(scale1_val, 5, 1, buf1); 
      dtostrf(scale2_val, 5, 1, buf2);
      dtostrf(scale3_val, 5, 1, buf3);
      dtostrf(scale4_val, 5, 1, buf4);
      
      draw_text(0, 0, buf1, 1);
      draw_text(64, 0, buf2, 1);
      draw_text(0, 32, buf3, 1);
      draw_text(64, 32, buf4, 1);
      display.display();
      
      float scale_front = scale1_val + scale2_val;
      float scale_back = scale3_val + scale4_val;

      if(abs(scale_front - scale_back) < 1.0)
      {
        display.clearDisplay();
        draw_text(0,0,"Done!!", 1);
        display.display();
        delay(2000);
        break;
      }
      else 
      {
        if(scale_front > scale_back)
          digitalWrite(dir, HIGH);
        else 
          digitalWrite(dir, LOW);
        
        if (currentMillis - previousStepTime >= stepInterval) {
            previousStepTime = currentMillis; 
            
            digitalWrite(step, HIGH);
            delayMicroseconds(500);
            digitalWrite(step, LOW);
        }
      }
    }
  }
}
