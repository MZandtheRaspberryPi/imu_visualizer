#!/usr/bin/python3

# SPDX-FileCopyrightText: 2021 ladyada for Adafruit Industries
# SPDX-License-Identifier: MIT

"""
This demo will fill the screen with white, draw a black box on top
and then print Hello World! in the center of the display

This example is for use on (Linux) computers that are using CPython with
Adafruit Blinka to support CircuitPython libraries. CircuitPython does
not support PIL/pillow (python imaging library)!
"""

from ctypes import CDLL, Structure, c_double
import math
import signal
import sys
import time

import board
import digitalio
from PIL import Image, ImageDraw, ImageFont
import adafruit_ssd1306

EXIT_FLAG = False

def signal_handler(sig, frame):
    global EXIT_FLAG
    print('You pressed Ctrl+C!')
    EXIT_FLAG = True

signal.signal(signal.SIGINT, signal_handler)

class FlattenedCoordinateFrameNonMatrix(Structure):

    _fields_ = [("x_start_x", c_double),
                ("x_start_y", c_double),
                ("x_end_x", c_double),
                ("x_end_y", c_double),
                ("y_start_x", c_double),
                ("y_start_y", c_double),
                ("y_end_x", c_double),
                ("y_end_y", c_double),
                ("z_start_x", c_double),
                ("z_start_y", c_double),
                ("z_end_x", c_double),
                ("z_end_y", c_double)
                ]

lib_vis = CDLL("libimu_visualizer_lib.so")

rotate_frame = lib_vis.rotate_frame

rotate_frame.argtypes = [c_double, c_double, c_double]
rotate_frame.restype = FlattenedCoordinateFrameNonMatrix

def rotate_frame_py(rot_x, rot_y, rot_z):
    rot_x_double = c_double(rot_x)
    rot_y_double = c_double(rot_y)
    rot_z_double = c_double(rot_z)

    return rotate_frame(rot_x_double, rot_y_double, rot_z_double)


# Define the Reset Pin
oled_reset = digitalio.DigitalInOut(board.D4)

# Change these
# to the right size for your display!
WIDTH = 128
HEIGHT = 64  # Change to 64 if needed
BORDER = 5

# Use for I2C.
i2c = board.I2C()  # uses board.SCL and board.SDA
# i2c = board.STEMMA_I2C()  # For using the built-in STEMMA QT connector on a microcontroller
oled = adafruit_ssd1306.SSD1306_I2C(WIDTH, HEIGHT, i2c, addr=0x3C, reset=oled_reset)

# Use for SPI
# spi = board.SPI()
# oled_cs = digitalio.DigitalInOut(board.D5)
# oled_dc = digitalio.DigitalInOut(board.D6)
# oled = adafruit_ssd1306.SSD1306_SPI(WIDTH, HEIGHT, spi, oled_dc, oled_reset, oled_cs)

# Clear display.
oled.fill(0)
oled.show()

# Create blank image for drawing.
# Make sure to create image with mode '1' for 1-bit color.
image = Image.new("1", (oled.width, oled.height))

# Get drawing object to draw on image.
draw = ImageDraw.Draw(image)

# Draw a white background
draw.rectangle((0, 0, oled.width, oled.height), outline=255, fill=255)

# Draw a smaller inner rectangle
draw.rectangle(
    (BORDER, BORDER, oled.width - BORDER - 1, oled.height - BORDER - 1),
    outline=0,
    fill=0,
)

# Load default font.
font = ImageFont.load_default()

# Draw Some Text
text = "Hello World!"
(font_width, font_height) = font.getsize(text)
draw.text(
    (oled.width // 2 - font_width // 2, oled.height // 2 - font_height // 2),
    text,
    font=font,
    fill=255,
)

# Display image
oled.image(image)
oled.show()

time.sleep(3)
# Clear display.
oled.fill(0)
oled.show()

rotation = 0
while not EXIT_FLAG:
    draw = ImageDraw.Draw(image)
    rotated_frame = rotate_frame_py(0, 0, rotation)
    rotation += 1 * math.PI / 180

    draw.line([(rotated_frame.x_start_x.value, rotated_frame.x_start_y.value), (rotated_frame.x_end_x.value,, rotated_frame.x_end_y.value)])

    oled.image(image)
    oled.show()

    time.sleep(0.010)