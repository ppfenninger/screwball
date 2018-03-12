"""created by anna on 3/12/18"""
import usb.core
from usbservo import usbservo
import time

class usbservo_functions:

    def __init__(self):
        self.controller = usbservo()
        self.running = False
        self.servo1_center = 35768
        self.servo2_center = 30768 #actual servo center is 32768
        self.servo1 = self.servo1_center
        self.servo2 = self.servo2_center
        self.servo_step = 500
        self.step_counts = 5
        self.d0 = self.controller.read_d0()
        self.d1 = self.controller.read_d1()
        self.d2 = self.controller.read_d2()
        self.d3 = self.controller.read_d3()
        self.d4 = self.controller.read_d4()
        self.servo1_limits = [self.servo1_center - self.step_counts*self.servo_step, self.servo1_center + 2*self.step_counts*self.servo_step]
        self.servo2_limits = [self.servo2_center - 2*self.step_counts*self.servo_step, self.servo2_center + self.step_counts*self.servo_step]


    def update_vals(self):
        self.d0 = self.controller.read_d0()
        self.d1 = self.controller.read_d1()
        self.d2 = self.controller.read_d2()
        self.d3 = self.controller.read_d3()
        self.d4 = self.controller.read_d4()
        self.controller.set_servo1(self.servo1)
        self.controller.set_servo2(self.servo2)

    def run(self):
        while True:
            self.update_vals()
            if (self.d0 == 0):
                self.controller.toggle_led1()
                self.running = True
            while self.running:
                self.update_vals()
                if ((self.d1 == 1) and (self.servo1 < self.servo1_limits[1])):
                    self.servo1 += self.servo_step
                    time.sleep(0.05)
                    self.prev_d1 = 1
                elif ((self.d2 == 1) and (self.servo1 > self.servo1_limits[0])):
                    self.servo1 += (-self.servo_step)
                    self.prev_d2 = 1
                    time.sleep(0.05)
                if ((self.d3 == 1) and (self.servo2 < self.servo2_limits[1])):
                    self.servo2 += self.servo_step
                    self.prev_d3 = 1
                    time.sleep(0.05)
                elif ((self.d4 == 1) and (self.servo2 > self.servo2_limits[0])):
                    self.servo2 += (-self.servo_step)
                    self.prev_d4 = 1
                    time.sleep(0.05)

if __name__ == '__main__':
    servo = usbservo_functions()
    servo.run()