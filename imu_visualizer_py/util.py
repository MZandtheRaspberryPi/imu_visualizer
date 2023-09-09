from ctypes import CDLL, Structure, c_double
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


lib_vis = CDLL(os.path.join(os.path.dirname(__file__), "libimu_visualizer_lib.so"))

rotate_frame = lib_vis.rotate_frame

rotate_frame.argtypes = [c_double, c_double, c_double]
rotate_frame.restype = FlattenedCoordinateFrameNonMatrix

def rotate_frame_py(rot_x, rot_y, rot_z):
    rot_x_double = c_double(rot_x)
    rot_y_double = c_double(rot_y)
    rot_z_double = c_double(rot_z)

    return rotate_frame(rot_x_double, rot_y_double, rot_z_double)