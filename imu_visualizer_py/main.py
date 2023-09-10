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

import math
import os
import signal
import sys
import time

from util import rotate_frame_py

from PIL import Image, ImageDraw, ImageFont
import matplotlib.pyplot as plt

RUNNING_ON_PI = os.uname()[4].startswith("arm")

if RUNNING_ON_PI:
    import board
    import digitalio
    import adafruit_ssd1306
    print("running on pi")

EXIT_FLAG = False
WIDTH = 128
HEIGHT = 64
BORDER = 5

def signal_handler(sig, frame):
    global EXIT_FLAG
    print('You pressed Ctrl+C!')
    EXIT_FLAG = True

signal.signal(signal.SIGINT, signal_handler)

if RUNNING_ON_PI:
    # Define the Reset Pin
    oled_reset = digitalio.DigitalInOut(board.D4)
    # Use for I2C.
    i2c = board.I2C()  # uses board.SCL and board.SDA
    oled = adafruit_ssd1306.SSD1306_I2C(WIDTH, HEIGHT, i2c, addr=0x3C, reset=oled_reset)

    # Clear display.
    oled.fill(0)
    oled.show()

# Create blank image for drawing.
# Make sure to create image with mode '1' for 1-bit color.
image = Image.new("1", (WIDTH, HEIGHT))

# Get drawing object to draw on image.
draw = ImageDraw.Draw(image)

# Draw a white background
draw.rectangle((0, 0, WIDTH, HEIGHT), outline=255, fill=255)

# Draw a smaller inner rectangle
draw.rectangle(
    (BORDER, BORDER, WIDTH - BORDER - 1, HEIGHT - BORDER - 1),
    outline=0,
    fill=0,
)

# Load default font.
font = ImageFont.load_default()

# Draw Some Text
text = "Hello World!"
(font_width, font_height) = font.getsize(text)
draw.text(
    (WIDTH// 2 - font_width // 2, HEIGHT // 2 - font_height // 2),
    text,
    font=font,
    fill=255,
)

if RUNNING_ON_PI:
    # Display image
    oled.image(image)
    oled.show()
else:
    image.show()

time.sleep(3)
if RUNNING_ON_PI:
    # Clear display.
    draw.rectangle( [(0,0), (oled.width, oled.height)], fill=0)
    oled.image(image)
    oled.show()
else:
    plt.imshow(image)
    plt.show(block=False)
    plt.pause(0.001)

rotation = 0
while not EXIT_FLAG:
    draw.rectangle( [(0,0), (WIDTH, HEIGHT)], fill=0)
    if RUNNING_ON_PI:
        oled.image(image)

    rotated_frame = rotate_frame_py(0, 0, rotation)
    rotation += 10 * math.pi / 180

    line_points_y = [(rotated_frame.y_start_x, rotated_frame.y_start_y), (rotated_frame.y_end_x, rotated_frame.y_end_y)]
    line_points_x = [(rotated_frame.x_start_x, rotated_frame.x_start_y), (rotated_frame.x_end_x, rotated_frame.x_end_y)]
    line_points_z =  [(rotated_frame.z_start_x, rotated_frame.z_start_y), (rotated_frame.z_end_x, rotated_frame.z_end_y)]

    all_line_points = line_points_x + line_points_y + line_points_z

    print("lines {}".format(all_line_points))

    draw.line(line_points_x, 1)
    draw.line(line_points_y, 1)
    draw.line(line_points_z, 1)

    if RUNNING_ON_PI:
        oled.image(image)
        oled.show()
    else:
        plt.imshow(image)
        plt.show(block=False)
        plt.pause(0.001)

    time.sleep(0.010)
