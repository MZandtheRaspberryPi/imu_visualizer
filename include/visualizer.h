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
  IMUVisualizer(const size_t &display_width, const size_t &display_height,
                const double &frame_rescale_factor = 1.5)
      : display_width_(display_width), display_height_(display_height),
        frame_rescale_factor_(frame_rescale_factor) {
    CoordinateFrame base_frame;
    base_frame.frame.setZero();
    base_frame.frame(1, 0) = 1;
    base_frame.frame(3, 1) = 1;
    base_frame.frame(5, 2) = -1;
    base_frame_ = base_frame;

    // our oled display has x as width of screen and y as height. Y increases
    // going to bottom of display. our world, looking down on it from
    // positive z axis, has x going up and down and y going right left.
    Eigen::Matrix<m_t, 3, 3> flip_x_y_matrix;
    flip_x_y_matrix.setZero();
    flip_x_y_matrix(1, 0) = 1;
    flip_x_y_matrix(0, 1) = -1;
    flip_x_y_matrix(2, 2) = 1;
    flip_x_y_matrix_ = flip_x_y_matrix;

    half_width_ = display_width_ / 2;
    half_height_ = display_height_ / 2;
  }

  CoordinateFrame get_rotated_coordinate_frame(m_t x_rotation, m_t y_rotation,
                                               m_t z_rotation) {

    // tricky here is that our z axis points downward
    // if we are in negative z, in our coordinate system,
    // and our camera points in positive z direction so we look down at
    // the coordinate system at our xs and ys, when we rotate positive z
    // rotation with no other rotation, we expect the x axis to
    // to move clockwise. That said, our input rotation from the filter
    // represents rotation from the global coordinate system to the filter
    // cordinate system. We want to undo that rotation when we visualize,
    // to show the world coordinate system and the z heading.
    // when we calculate rotation matrix and rotate our coordinate frame
    // we haven't put in the info that we flipped our z axis from up to down
    // hence we would need to flip rotation as well. By not telling it that info
    // we get this flip of rotation sign.
    RotationMatrix rotation_matrix =
        get_xyz_rotation(x_rotation, y_rotation, z_rotation);

    CoordinateFrame new_frame = base_frame_;
    new_frame.frame = new_frame.frame * rotation_matrix;
    return new_frame;
  }

  FlattenedCoordinateFrame
  flatten_coordinate_frame(const CoordinateFrame &frame) {

    // https://stackoverflow.com/questions/724219/how-to-convert-a-3d-point-into-2d-perspective-projection

    // // converting to xyzw coords
    Eigen::Matrix<m_t, 6, 4> existing_coords;
    existing_coords.setIdentity();
    existing_coords(Eigen::seq(0, 5), Eigen::seq(0, 2)) = frame.frame;
    Eigen::Matrix<m_t, 6, 1> ones;
    ones.setOnes();
    existing_coords(Eigen::seq(0, 5), 3) = ones;
    // scale our coordinate frames down so they don't take up so much space on
    // the screen
    existing_coords(Eigen::seq(0, 5), Eigen::seq(0, 2)) =
        existing_coords(Eigen::seq(0, 5), Eigen::seq(0, 2)).array() /
        frame_rescale_factor_;

    // our screen has x as the width of screen and y as height
    // whereas our coordinate system is reversed.
    existing_coords(Eigen::seq(0, 5), Eigen::seq(0, 2)) =
        existing_coords(Eigen::seq(0, 5), Eigen::seq(0, 2)) * flip_x_y_matrix_;

    // screen projection from coordinate to 2d screen
    FlattenedCoordinateFrame new_frame;
    new_frame.frame.setZero();
    new_frame.frame(Eigen::seq(0, 5), 0) =
        (existing_coords(Eigen::seq(0, 5), 0) *
         static_cast<double>(display_width_))
                .array() /
            ((2.0F * existing_coords(Eigen::seq(0, 5), 3))).array() +
        half_width_;

    new_frame.frame(Eigen ::seq(0, 5), 1) =
        (existing_coords(Eigen ::seq(0, 5), 1) *
         static_cast<double>(display_height_))
                .array() /
            ((2.0F * existing_coords(Eigen::seq(0, 5), 3))).array() +
        half_height_;
    return new_frame;
  }

private:
  size_t display_width_;
  size_t display_height_;

  m_t half_width_;
  m_t half_height_;
  double frame_rescale_factor_;

  CoordinateFrame base_frame_;
  Eigen::Matrix<m_t, 3, 3> flip_x_y_matrix_;
  Eigen::Matrix<m_t, 4, 4> zoom_out_matrix_;
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
