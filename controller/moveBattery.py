### HX711
### loadcell
### raspberry pi 4
"""
pinmap
HX711
DT -> GPIO 5
SCK -> GPIO 6

DT -> GPIO 23
SCK -> GPIO 24
"""

#pip3 install hx711

import RPi.GPIO as GPIO
import time
from hx711 import HX711
from moveStepper import moveStepper

class move_battery :
    def __init__(self):
        self.DT1 = 29
        self.SCK1 = 31
        self.CalFac1 = 0 # put digital data per gram
        self.DT2 = 16
        self.SCK2 = 18
        self.CalFac2 = 0  # put digital data per gram
        GPIO.cleanup()
        time.sleep(1)
        GPIO.setmode(GPIO.BOARD)
        self.hx1 = HX711(self.DT1, self.SCK1)
        self.hx2 = HX711(self.DT2, self.SCK2)

    def setup(self):
        moveStepper.setup()
        print("Setting zero factor")
        self.hx1.reset()
        self.hx1.tare()
        self.hx2.reset()
        self.hx2.tare()
        print("Zero factor set!s")
        self.hx1.set_reference_unit(self.CalFac1)
        self.hx2.set_reference_unit(self.CalFac2)
        print("end loadcell setup!")
    def goto_pos(self, cog):
        print('cog point has given to moveBattery.py', cog)
        print('Going to cog point')
        hx1_val = self.hx1.get_weight(5)
        hx2_val = self.hx2.get_weight(5)
        while True :
            if (abs(hx1_val-hx2_val)<1) :
                break
            if (hx1_val>hx2_val) :
                moveStepper.setDir(True)
            else :
                moveStepper.setDir(False)
            moveStepper.moveOneStep()
        print("End go to cog point!")
        if cog==0 :
            print("end!!")
            return
        if cog<0 :
            moveStepper.setDir(True)
            while True:
                if (hx1_val - hx2_val<cog):
                    break
                moveStepper.moveOneStep()
            print("end!!")
            return
        moveStepper.setDir(False)
        while True:
            if (hx1_val - hx2_val>cog):
                break
            moveStepper.moveOneStep()
        print("end!!")
        return