#!/usr/bin/python3

"""
This script will start listening to the output of the filter via websockets, and display this on the OLED screen.
"""

import math
import os
import signal
import sys
import time

from util import rotate_frame_py, start_listening, stop_listening, ImuMsgVis, get_latest_imu_msg, is_calibrated, get_calibration_string

from PIL import Image, ImageDraw, ImageFont
import matplotlib.pyplot as plt

CHECK_CALIBRATION = True
USE_FILTER_CALCULATED_ESTIMATES = True
DEBUG_MODE = True
EXIT_FLAG = False
WIDTH = 128
HEIGHT = 64
BORDER = 5
FONT_SIZE = 10

RUNNING_ON_PI = os.uname()[4].startswith("arm") or os.uname()[4].startswith("aarch64")
# Create blank image for drawing.
# Make sure to create image with mode '1' for 1-bit color.
IMAGE = Image.new("1", (WIDTH, HEIGHT))

# Get drawing object to draw on image.
DRAW = ImageDraw.Draw(IMAGE)

if RUNNING_ON_PI:
    import board
    import digitalio
    import adafruit_ssd1306
    print("running on pi")

def show():
    global IMAGE
    if RUNNING_ON_PI:
        oled.image(IMAGE)
        oled.show()
    else:
        plt.imshow(IMAGE)
        plt.show(block=False)
        plt.pause(0.001)


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

# Draw a white background
DRAW.rectangle((0, 0, WIDTH, HEIGHT), outline=255, fill=255)

# Draw a smaller inner rectangle
DRAW.rectangle(
    (BORDER, BORDER, WIDTH - BORDER - 1, HEIGHT - BORDER - 1),
    outline=0,
    fill=0,
)

font = ImageFont.truetype(os.path.join(os.path.dirname(__file__), "arial.ttf"), FONT_SIZE)

# Draw Some Text
text = "Hello World!"
(_, _, font_width, font_height) = font.getbbox(text)
DRAW.text(
    (WIDTH// 2 - font_width // 2, HEIGHT // 2 - font_height // 2),
    text,
    font=font,
    fill=255,
)

show()

time.sleep(10)
start_listening()

time.sleep(3)
DRAW.rectangle( [(0,0), (oled.width, oled.height)], fill=0)
show()

rotation = 0
while not EXIT_FLAG:
    DRAW.rectangle( [(0,0), (WIDTH, HEIGHT)], fill=0)

    imu_msg: ImuMsgVis = get_latest_imu_msg()

    if CHECK_CALIBRATION:

        if not is_calibrated(imu_msg):
            calib_str = get_calibration_string(imu_msg)
            DRAW.text((0, 0),
                  calib_str,
                  font=font,
                  fill=255)
            show()
            continue

    rotation = [0, 0, 0]
    if imu_msg.has_msg:
        if USE_FILTER_CALCULATED_ESTIMATES:
            rotation[0] = imu_msg.euler_angles_filter.x * math.pi / 180
            rotation[1] = imu_msg.euler_angles_filter.y * math.pi / 180
            rotation[2] = imu_msg.euler_angles_filter.z * math.pi / 180
        else:
            rotation[0] = imu_msg.euler_angles.x * math.pi / 180
            rotation[1] = imu_msg.euler_angles.y * math.pi / 180
            rotation[2] = imu_msg.euler_angles.z * math.pi / 180
        if DEBUG_MODE:
            for i, axis_letter in zip(range(3), ["x", "y", "z"]): 
                DRAW.text(
                  (0, i*font_height),
                  "{}: {}pi".format(axis_letter, round(rotation[i]/math.pi, 2)),
                  font=font,
                  fill=255)

    rotated_frame = rotate_frame_py(rotation[0], rotation[1], rotation[2])

    line_points_y = [(rotated_frame.y_start_x, rotated_frame.y_start_y), (rotated_frame.y_end_x, rotated_frame.y_end_y)]
    line_points_x = [(rotated_frame.x_start_x, rotated_frame.x_start_y), (rotated_frame.x_end_x, rotated_frame.x_end_y)]
    line_points_z =  [(rotated_frame.z_start_x, rotated_frame.z_start_y), (rotated_frame.z_end_x, rotated_frame.z_end_y)]

    all_line_points = line_points_x + line_points_y + line_points_z

    for axis_letter, axis_index in zip(["x", "y", "z"], range(3)):
        start_index = axis_index * 2
        end_index = axis_index * 2 + 1
        DRAW.line([all_line_points[start_index], all_line_points[end_index]], 1)
        end_x, end_y = all_line_points[end_index][0], all_line_points[end_index][1]
        DRAW.text(
            (end_x, end_y),
             axis_letter,
            font=font,
            fill=255)

    show()

    time.sleep(0.010)

stop_listening()
DRAW.rectangle( [(0,0), (oled.width, oled.height)], fill=0)
show()
