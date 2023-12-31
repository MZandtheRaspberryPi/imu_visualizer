from ctypes import CDLL, Structure, c_double, c_bool, c_uint64, c_uint32
import os
from typing import Tuple

class FlattenedCoordinateFrameNonMatrix(Structure):

    _fields_ = [("x_start_x", c_double),
                ("x_start_y", c_double),
                ("y_start_x", c_double),
                ("y_start_y", c_double),
                ("z_start_x", c_double),
                ("z_start_y", c_double),
                ("x_end_x", c_double),
                ("x_end_y", c_double),
                ("y_end_x", c_double),
                ("y_end_y", c_double),
                ("z_end_x", c_double),
                ("z_end_y", c_double)
                ]

class TriadVis(Structure):

    _fields_ = [("x", c_double),
                ("y", c_double),
                ("z", c_double)]

class QuaternionVis(Structure):

    _fields_ = [("w", c_double),
                ("xyz", TriadVis)]

class ImuMsgVis(Structure):

    _fields_ = [("has_msg", c_bool),
                ("timestamp", c_uint64),
                ("ground_truth", QuaternionVis),
                ("euler_angles", TriadVis),
                ("linear_acceleration", TriadVis),
                ("angular_acceleration", TriadVis),
                ("magnetometer_vector", TriadVis),
                ("board_temp", c_uint32),
                ("system_calibration", c_uint32),
                ("gyro_calibration", c_uint32),
                ("accel_calibration", c_uint32),
                ("mag_calibration", c_uint32),
                ("filter_timestamp", c_uint64),
                ("euler_angles_filter", TriadVis),
                ("covariance_matrix_trace", c_double)]


lib_vis = CDLL(os.path.join(os.path.dirname(__file__), "libimu_visualizer_lib.so"))

rotate_frame = lib_vis.rotate_frame
rotate_frame.argtypes = [c_double, c_double, c_double]
rotate_frame.restype = FlattenedCoordinateFrameNonMatrix

start_listening = lib_vis.start_listening
start_listening.argtypes = []
start_listening.restype = None

stop_listening = lib_vis.stop_listening
stop_listening.argtypes = []
stop_listening.restype = None

get_latest_imu_msg = lib_vis.get_latest_imu_msg
get_latest_imu_msg.argtypes = []
get_latest_imu_msg.restype = ImuMsgVis


def rotate_frame_py(rot_x, rot_y, rot_z):
    rot_x_double = c_double(rot_x)
    rot_y_double = c_double(rot_y)
    rot_z_double = c_double(rot_z)

    return rotate_frame(rot_x_double, rot_y_double, rot_z_double)


def is_calibrated(imu_msg: ImuMsgVis):

    return imu_msg.system_calibration >= 2 # and imu_msg.gyro_calibration==3 and imu_msg.accel_calibration == 3 and imu_msg.mag_calibration == 3


def get_calibration_string(imu_msg: ImuMsgVis):
    calib_str = "Please calibrate:\n"
    calib_str += "sys: {}\n".format(imu_msg.system_calibration)
    calib_str += "gyro: {}\n".format(imu_msg.gyro_calibration)
    calib_str += "accel: {}\n".format(imu_msg.accel_calibration)
    calib_str += "mag: {}\n".format(imu_msg.mag_calibration)
    return calib_str

def get_mock_ImuMsgVis(x_rotation_degrees: float, y_rotation_degrees: float, z_rotation_degrees: float):
    msg = ImuMsgVis()
    msg.system_calibration = c_uint32(3)
    msg.gyro_calibration = c_uint32(3)
    msg.accel_calibration = c_uint32(3)
    msg.mag_calibration = c_uint32(3)

    euler_angles = TriadVis()
    euler_angles.x = c_double(x_rotation_degrees)
    euler_angles.y = c_double(y_rotation_degrees)
    euler_angles.z = c_double(z_rotation_degrees)

    msg.euler_angles = euler_angles
    msg.euler_angles_filter = euler_angles

    msg.has_msg = c_bool(True)

    msg.covariance_matrix_trace = 5000

    return msg

def parse_xyz_input(x_y_z_rot_str: str) -> Tuple[float, float, float]:
    err_res = (0, 0, 0)
    example_str = "0,0,30"

    if x_y_z_rot_str.count(",") != 2:
        print("did not input valid string, example: " + example_str)
        return err_res
    split_rotations = x_y_z_rot_str.split(",")
    if len(split_rotations) != 3:
        print("did not input valid string, example: " + example_str)
        return err_res
    parsed_rotations = []
    try:
        parsed_rotations = [float(val) for val in split_rotations]
    except ValueError as err:
        print("did not input valid string, example: " + example_str)
        return err_res
    return (parsed_rotations[0], parsed_rotations[1], parsed_rotations[2])
