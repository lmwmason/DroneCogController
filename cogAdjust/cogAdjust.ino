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

void draw_text(byte x_pos, byte y_pos, char *text, byte text_size) {
  display.setCursor(x_pos, y_pos);
  display.setTextSize(text_size);
  display.print(text);
  display.display();
}

void setup() {
  Serial.begin(9600);

  pinMode(bt, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE, BLACK);
  draw_text(0,0,"display set end!", false);
  delay(2000);

  display.clearDisplay();
  pinMode(enable, OUTPUT);
  pinMode(step, OUTPUT);
  pinMode(dir, OUTPUT);
  digitalWrite(enable, LOW);
  digitalWrite(step, LOW);
  digitalWrite(dir, LOW);
  draw_text(0,0,"stepper set end!", false);
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
  draw_text(0,0,"stepper set end!", false);
  delay(2000);
  display.clearDisplay();
}

void loop() {
  display.clearDisplay();
  draw_text(0,0,"Ready....", false);
  if(analogRead(bt)==0)
  {
    display.clearDisplay();
    draw_text(0,0,"bt Clicked!", false);
    delay(1000);
    while(1)
    {
      display.clearDisplay();
      int scale1_val = (int)scale1.get_units();
      int scale2_val = (int)scale2.get_units();
      int scale3_val = (int)scale3.get_units();
      int scale4_val = (int)scale4.get_units();
      draw_text(0,0,char(scale1_val), false);
      draw_text(64,0,char(scale2_val), false);
      draw_text(0,32,char(scale3_val), false);
      draw_text(64,32,char(scale4_val), false);
      int scale_front = scale1_val+scale2_val;
      int scale_back = scale3_val+scale4_val;
      if(abs(scale_front-scale_back)<3)
      {
        display.clearDisplay();
        draw_text(0,0,"Done!!", false);
        delay(2000);
        break;
      }
      else if(scale_front>scale_back)
        digitalWrite(dir, HIGH);
      else if(scale_front<scale_back)
        digitalWrite(dir, LOW);
      digitalWrite(step, HIGH);
      delay(10);
      digitalWrite(step, LOW);
      delay(10);
    }
  }
}
