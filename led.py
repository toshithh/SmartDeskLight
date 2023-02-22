from machine import Pin, PWM
import time

class Led:
    def __init__(self, p1, p2, p3, freq=1000):
        self.R = PWM(Pin(p1))
        self.G = PWM(Pin(p2))
        self.B = PWM(Pin(p3))
        self.colorValues = [0,0,0]
        self.R.freq(freq)
        self.G.freq(freq)
        self.B.freq(freq)

    def duty(self, values):
        self.R.duty_u16(int(255*(255-values[0])))
        self.G.duty_u16(255*(255-values[1]))
        self.B.duty_u16(255*(255-values[2]))
        self.colorValues = values

    @property
    def color(self):
        return self.colorValues

    @color.setter
    def color(self, color):
        self.duty(color)
