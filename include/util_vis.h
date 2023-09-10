#pragma once

#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>

typedef double m_t;
typedef uint8_t byte;
typedef std::pair<size_t, size_t> DisplayScreenCoord;
typedef Eigen::Matrix<m_t, 3, 3> RotationMatrix;

struct CoordinateFrame {
  // 6 x3 matrix, 2 rows per x, y, z. x represented by starting point, ending
  // point of line.
  Eigen::Matrix<m_t, 6, 3> frame;
  size_t x_starting_index = 0;
  size_t x_ending_index = 1;
  size_t y_starting_index = 2;
  size_t y_ending_index = 3;
  size_t z_starting_index = 4;
  size_t z_ending_index = 5;
};

struct FlattenedCoordinateFrame {
  // 6 x2 matrix, 2 rows per x, y, z. x represented by starting point, ending
  // point of line.
  Eigen::Matrix<m_t, 6, 2> frame;
  size_t x_starting_index = 0;
  size_t x_ending_index = 1;
  size_t y_starting_index = 2;
  size_t y_ending_index = 3;
  size_t z_starting_index = 4;
  size_t z_ending_index = 5;
};

struct FlattenedCoordinateFrameNonMatrix {
  // 6 x2 matrix, 2 rows per x, y, z. x represented by starting point, ending
  // point of line.
  double x_start_x;
  double x_start_y;
  double y_start_x;
  double y_start_y;
  double z_start_x;
  double z_start_y;
  double x_end_x;
  double x_end_y;
  double y_end_x;
  double y_end_y;
  double z_end_x;
  double z_end_y;
};

struct TriadVis {
  double x;
  double y;
  double z;
};

struct QuaternionVis {
  double w;
  TriadVis xyz;
  ;
};

typedef std::vector<std::vector<double>> CovarianceMatrixVis;

struct ImuMsgVis {
  bool has_msg;
  uint64_t timestamp;
  QuaternionVis ground_truth;
  TriadVis euler_angles;
  TriadVis linear_acceleration;
  TriadVis angular_acceleration;
  TriadVis magnetometer_vector;
  uint32_t board_temp;
  uint32_t system_calibration;
  uint32_t gyro_calibration;
  uint32_t accel_calibration;
  uint32_t mag_calibration;
  uint64_t filter_timestamp;
  CovarianceMatrixVis euler_angles_filter;
};

bool get_bit(const byte *arr, const size_t &position);
void set_bit(const byte *arr, const size_t &index);

m_t do_cos(m_t angle) { return cos(angle); }
m_t do_sin(m_t angle) { return sin(angle); }
m_t do_tan(m_t angle) { return tan(angle); }

RotationMatrix get_x_rotation(m_t rotation_rad) {
  RotationMatrix rotation;
  rotation.setZero();
  rotation(0, 0) = 1;
  rotation(1, 1) = cos(rotation_rad);
  rotation(1, 2) = -sin(rotation_rad);
  rotation(2, 1) = sin(rotation_rad);
  rotation(2, 2) = cos(rotation_rad);
  return rotation;
}

RotationMatrix get_y_rotation(m_t rotation_rad) {
  RotationMatrix rotation;
  rotation.setZero();
  rotation(1, 1) = 1;
  rotation(0, 0) = cos(rotation_rad);
  rotation(0, 2) = sin(rotation_rad);
  rotation(2, 0) = -sin(rotation_rad);
  rotation(2, 2) = cos(rotation_rad);
  return rotation;
}

RotationMatrix get_z_rotation(m_t rotation_rad) {
  RotationMatrix rotation;
  rotation.setZero();
  rotation(2, 2) = 1;
  rotation(0, 0) = cos(rotation_rad);
  rotation(0, 1) = -sin(rotation_rad);
  rotation(1, 0) = sin(rotation_rad);
  rotation(1, 1) = cos(rotation_rad);
  return rotation;
}

RotationMatrix get_xyz_rotation(m_t x_rot, m_t y_rot, m_t z_rot) {
  RotationMatrix x_rotation = get_x_rotation(x_rot);
  RotationMatrix y_rotation = get_y_rotation(y_rot);
  RotationMatrix z_rotation = get_z_rotation(z_rot);
  return z_rotation * (y_rotation * x_rotation);
}

Eigen::Matrix<m_t, 4, 4> get_clip_matrix(m_t fov, m_t aspect_ratio, m_t near,
                                         m_t far) {
  // https://stackoverflow.com/questions/724219/how-to-convert-a-3d-point-into-2d-perspective-projection
  Eigen::Matrix<m_t, 4, 4> clip_matrix;
  clip_matrix.setIdentity();
  float f = 1.0f / do_tan(fov * 0.5f);
  clip_matrix(0, 0) = f * aspect_ratio;
  clip_matrix(1, 1) = f;
  clip_matrix(2, 2) = (far + near) / (far - near);
  clip_matrix(2, 3) = 1;
  clip_matrix(3, 2) = (2.0f * near * far) / (near - far);
  clip_matrix(3, 3) = 0;
  return clip_matrix;
}