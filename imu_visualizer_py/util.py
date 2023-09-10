from ctypes import CDLL, Structure, c_double, c_bool, c_uint64, c_uint32
import os

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
                ("filter_timestamp", c_uint64)]


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