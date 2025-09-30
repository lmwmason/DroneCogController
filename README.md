# Drone Center of Gravity (CoG) Controller  

This project implements a **drone Center of Gravity (CoG) controller** using four load cells (HX711), a stepper motor (TMC2225/A4988 driver), and an OLED display (SSD1306).  
The system dynamically detects weight distribution on the drone frame and adjusts the CoG by moving a counterweight with a stepper motor, ensuring stable and balanced flight.  

---

## Features  
- **Real-time CoG Measurement:** Four load cells detect weight distribution at each corner of the drone.  
- **PID Control for CoG Adjustment:** Automatically calculates front–back balance error and corrects it with the stepper motor.  
- **OLED Display:** Shows live sensor data from each load cell.  
- **Start/Stop Button:** Single button for initiating CoG adjustment or emergency stop.  
- **Safety Timeout:** Prevents the motor from running indefinitely.  
- **System Messages:** Displays system status (ready, balancing, error, timeout, emergency stop).  

---

## Hardware Requirements  
- Arduino Uno / Nano (or compatible board)  
- 4 × Load cells with HX711 amplifiers (mounted on drone frame corners)  
- Stepper motor + Driver (TMC2225, A4988, or compatible) for moving counterweight  
- OLED Display (SSD1306, I²C) for live data  
- Push button for control  
- External power supply for stepper motor (e.g., 12V battery system)  

---

## Wiring Overview  

**Stepper Motor Driver**  
- D2 → ENABLE  
- D3 → STEP  
- D4 → DIR  

**Load Cells (HX711 modules)**  
- Front Left:  D5 (DOUT), D6 (CLK)  
- Front Right: D7 (DOUT), D8 (CLK)  
- Back Left:   D9 (DOUT), D10 (CLK)  
- Back Right:  D11 (DOUT), D12 (CLK)  

**OLED Display (I²C)**  
- SDA → A4  
- SCL → A5  
- VCC → 3.3V / 5V  
- GND → GND  

**Button**  
- D13 → Button input (with `INPUT_PULLUP`)  
- GND → Button output  

---

## Software Requirements  
Install the following Arduino libraries:  
- [HX711](https://github.com/bogde/HX711)  
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)  
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)  

---

## How It Works  
1. Four load cells continuously measure forces at each corner of the drone.  
2. The system calculates the difference between the **front** and **back** loads.  
3. A PID controller generates correction values.  
4. The stepper motor moves the counterweight until the CoG is within the threshold.  
5. The cycle ends when:  
   - The error is corrected (balanced),  
   - The maximum time expires, or  
   - The button is pressed (emergency stop).  

---

## Usage  
1. Mount load cells under the drone’s four landing legs.  
2. Connect the counterweight mechanism to the stepper motor.  
3. Wire the hardware according to the schematic above.  
4. Upload the code to Arduino.  
5. Power on the system — OLED shows “Ready…”.  
6. Press the button to start CoG balancing.  

---

## Safety Notes  
- Ensure all grounds are connected (Arduino, HX711, motor driver, power source).  
- Use proper vibration damping for load cells.  
- Stepper motor should only move the counterweight, not interfere with propeller thrust.  
- Never run the adjustment mechanism while in-flight — **use this system for pre-flight balancing only**.  
