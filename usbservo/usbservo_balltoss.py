"""created by anna on 4/20/18"""
import usb.core
from usbservo import usbservo
import time

class usbservo_balltoss:

    def __init__(self):
        self.controller = usbservo()
        self.running = False
        self.servo1_center = 35768
        self.servo2_center = 30768 #actual servo center is 32768
        self.servo1 = self.servo1_center
        self.servo2 = self.servo2_center
        self.servo_step = 500
        self.step_counts = 5
        self.a0 = self.controller.read_a0()
        self.a1 = self.controller.read_a1()
        self.servo1_limits = [0, 65535]
        self.servo2_limits = [0, 65535]


    def update_vals(self):
        self.a0 = self.controller.read_a0()
        self.a1 = self.controller.read_a1()
        self.controller.set_servo1(self.servo1)
        self.controller.set_servo2(self.servo2)

    def map_to_servo(self,val):
        PotRange = 1023 
        ServoRange = 65535
        return ((val * ServoRange) / PotRange)

    def run(self):
        self.update_vals()
        self.running = True
        while self.running:
            self.update_vals()
            self.servo1 = self.map_to_servo(self.a0)
            self.servo2 = self.map_to_servo(self.a1)
            time.sleep(0.05)

if __name__ == '__main__':
    servo = usbservo_balltoss()
    servo.run()