from machine import Pin,UART
import time
from led import *

uart = UART(1, baudrate=115200, tx=Pin(4), rx=Pin(5))
uart.init(bits=8, parity=None, stop=2)
left = Led(10,12,13)
left.color=[0,0,0]
right = Led(4,3,2)
right.color=[0,0,0]

while True:
    if uart.any():
        data = uart.read()
        try:
            data = str(data.decode())
            data = data.replace("\r","")
            data = data.replace("\n", "")
            data = data.replace("(", "")
            data = data.replace(")", "")
            data = data.split(",")
            data = [int(x) for x in data]
            left.color = data
            right.color = data
            print(data)
        except:
            continue
        if data:
            right.color = data
            left.color = data
    time.sleep(1)