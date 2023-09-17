#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Dense>

#include "util_vis.h"
#include "websocket_listener.h"

class DisplayScreen {

public:
  DisplayScreen(size_t display_width, size_t display_height)
      : display_width_(display_width), display_height_(display_height) {

    size_t num_bytes = display_width_ * display_height_ / 8;
    if ((display_width * display_height) % 8 != 0) {
      num_bytes++;
    }

    byte_arr_ = std::shared_ptr<byte[]>(new byte[num_bytes]);
  }
  ~DisplayScreen();

  byte *get_arr_ptr() { return byte_arr_.get(); }

  void set_pixel(const size_t &width_index, const size_t &height_index);

private:
  size_t get_arr_index(const size_t &width_index, const size_t &height_index);

  size_t display_width_;
  size_t display_height_;

  // it may be faster to store indices that we need to update so we can update
  // pixels instead of updating the whole screen. But, this comes at a
  // signficant memory cost. If 128x64 pixel display, we have 128 * 64 pixels, *
  // 64 bits for a size_t * 2 for x, y which ends up with 128kb. We will need to
  // investigate this.
  std::vector<DisplayScreenCoord> set_pixel_coords_;
  std::shared_ptr<byte[]> byte_arr_;
};

// we want to do compass like circle with dashes around side at 10 degree
// intervals then perhaps left of circle is our estimate, right of circle is
// oponents estimate. if we don't have opponents then we jsut have ours...

// we could do a line class, has thickness, has start and end
// or we do a matrix of points, and we rotate that around the circle...

// do we set zero zero to middle of screen? or left hand side?

// actually we really just need lines...shall we do a 2d line and a 3d line?

class IMUVisualizer {
public:
  IMUVisualizer(const size_t &display_width, const size_t &display_height)
      : display_width_(display_width), display_height_(display_height) {
    CoordinateFrame base_frame;
    base_frame.frame.setZero();
    base_frame.frame(1, 0) = 1;
    base_frame.frame(3, 1) = 1;
    base_frame.frame(5, 2) = -1;
    base_frame_ = base_frame;

    // our oled display has x as width of screen and y as height
    // our world has x, looking down on it from positive z axis, has x going up
    // and down and y going right left.
    Eigen::Matrix<m_t, 2, 2> flip_x_y_matrix;
    flip_x_y_matrix.setZero();
    flip_x_y_matrix(1, 0) = 1;
    flip_x_y_matrix(0, 1) = 1;
    flip_x_y_matrix_ = flip_x_y_matrix;
  }

  CoordinateFrame get_rotated_coordinate_frame(m_t x_rotation, m_t y_rotation,
                                               m_t z_rotation) {
    RotationMatrix rotation_matrix =
        get_xyz_rotation(x_rotation, y_rotation, z_rotation);

    CoordinateFrame new_frame = base_frame_;
    new_frame.frame = new_frame.frame * rotation_matrix;
    return new_frame;
  }

  FlattenedCoordinateFrame
  flatten_coordinate_frame(const CoordinateFrame &frame) {

    // https://stackoverflow.com/questions/724219/how-to-convert-a-3d-point-into-2d-perspective-projection

    m_t half_width = display_width_ / 2;
    m_t half_height = display_height_ / 2;

    Eigen::Matrix<m_t, 4, 4> clip_matrix =
        get_clip_matrix(M_PI / 2, display_width_ / display_height_, -1, 2);

    // converting to xyzw coords
    Eigen::Matrix<m_t, 6, 4> existing_coords;
    existing_coords.setIdentity();
    existing_coords(Eigen::seq(0, 5), Eigen::seq(0, 2)) = frame.frame;
    Eigen::Matrix<m_t, 6, 1> ones;
    ones.setOnes();
    existing_coords(Eigen::seq(0, 5), 3) = ones;

    // doing inverse camera matrix transformation
    // our screen has x as width and y as length, so we need to rotate pi / 2,
    // 90 degrees, around z to correct
    Eigen::Matrix<m_t, 4, 4> inverse_camera_matrix;
    inverse_camera_matrix.setIdentity();
    /*
        inverse_camera_matrix(2, 3) = 3;

        inverse_camera_matrix(Eigen::seq(0, 2), Eigen::seq(0, 2)) =
            get_y_rotation(M_PI / 8) * get_x_rotation(M_PI) *
            get_z_rotation(M_PI / 2);
    */
    // std::cout << "inverse cam matrix:\n" << inverse_camera_matrix <<
    // std::endl;
    existing_coords = existing_coords * inverse_camera_matrix;

    // std::cout << "existing coord post cam, pre norm\n"
    //           << existing_coords << std::endl;

    int r = static_cast<int>(existing_coords.rows());

    for (int i = 0; i < r; i++) {
      existing_coords(i, Eigen::seq(0, 2)) =
          existing_coords(i, Eigen::seq(0, 2)) / existing_coords(i, 3);
    }
    // std::cout << "existing coord post norm\n" << existing_coords <<
    // std::endl;

    // applying clip matrix
    Eigen::Matrix<m_t, 6, 4> new_points;
    new_points.setZero();

    new_points = existing_coords * clip_matrix;
    // std::cout << "post clip matrix\n" << new_points << std::endl;

    r = static_cast<int>(new_points.rows());

    for (int i = 0; i < r; i++) {
      new_points(i, Eigen::seq(0, 2)) =
          new_points(i, Eigen::seq(0, 2)) / new_points(i, 3);
    }

    // std::cout << "post clip, post norm\n" << new_points << std::endl;

    // screen projection from coordinate to 2d screen
    FlattenedCoordinateFrame new_frame;
    new_frame.frame.setZero();
    new_frame.frame(Eigen::seq(0, 5), 0) =
        (existing_coords(Eigen::seq(0, 5), 0) *
         static_cast<double>(display_width_))
                .array() /
            ((2.0F * existing_coords(Eigen::seq(0, 5), 3))).array() +
        half_width;

    new_frame.frame(Eigen ::seq(0, 5), 1) =
        (existing_coords(Eigen ::seq(0, 5), 1) *
         static_cast<double>(display_height_))
                .array() /
            ((2.0F * existing_coords(Eigen::seq(0, 5), 3))).array() +
        half_height;

    new_frame.frame = new_frame.frame * flip_x_y_matrix_;
    return new_frame;
  }

private:
  size_t display_width_;
  size_t display_height_;
  CoordinateFrame base_frame_;
  Eigen::Matrix<m_t, 2, 2> flip_x_y_matrix_;
};

void proto_msg_to_c_struct(const imu_msgs::ImuMsg &msg, ImuMsgVis &msg_vis);
std::shared_ptr<ListenerClient> get_listener();

extern "C" {
FlattenedCoordinateFrameNonMatrix
rotate_frame(double x_rotation, double y_rotation, double z_rotation);

void start_listening();

void stop_listening();
ImuMsgVis get_latest_imu_msg();
}
