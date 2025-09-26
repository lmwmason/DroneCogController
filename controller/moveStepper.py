### motor driver : tmc2225
### stepper motor : nema17
### raspberry pi 4
"""
pinmap
tmc2225
STEP -> GPIO 17
DIR -> GPIO 27
ENABLE -> GPIO 22
"""

import RPi.GPIO as GPIO
import time

class moveStepper :
    def __init__(self):
        self.step = 11
        self.dir = 13
        self.enable = 15
    def setup(self):
        GPIO.cleanup()
        time.sleep(1)
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(self.step, GPIO.OUT)
        GPIO.setup(self.dir, GPIO.OUT)
        GPIO.setup(self.enable, GPIO.OUT)
        GPIO.output(self.step, GPIO.LOW)
        GPIO.output(self.dir, GPIO.LOW)
        GPIO.output(self.enable, GPIO.LOW)
        print("end stepper setup!")
    def setDir(self, direction):
        GPIO.output(self.dir, GPIO.HIGH if direction else GPIO.LOW)
    def moveOneStep(self):
        GPIO.output(self.step, GPIO.HIGH)
        time.sleep(0.1)
        GPIO.output(self.step, GPIO.LOW)
        time.sleep(0.1)