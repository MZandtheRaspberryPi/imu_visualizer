#pragma once

#include <memory>
#include <pair>
#include <vector>

#include <Eigen/Dense>

typedef double m_t;
typedef uint8_t byte;
typedef std::pair<size_t, size_t> DisplayScreenCoord;

bool get_bit(const byte *arr, const size_t &position);
void set_bit(const byte *arr, const size_t &index);

class DisplayScreen {

public:
  DisplayScreen(size_t display_width, size_t display_height)
      : display_width_(display_width), display_height_(display_height) {

    size_t num_bytes = display_width_ * display_height_ / 8;
    if ((display_width * display_height) % 8 != 0) {
      num_bytes++;
    }

    byte_arr_(new byte[num_bytes]);
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

struct CoordinateFrame {
  Line3D x_line;
  Line3D y_line;
  Line3D z_line;
};

class IMUVisualizer {
public:
  IMUVisualizer(size_t display_width, size_t display_height)
      : display_width_(display_width), display_height_(display_height) {}

private:
  CoordinateFrame get_base_coordinate_frame() {
    CoordinateFrame base_frame;
    XYZ zero_origin;
    zero_origin.x = 0;
    zero_origin.y = 0;
    zero_origin.z = 0;
    base_frame.x_starting_coord = zero_origin;
    base_frame.y_starting_coord = zero_origin;
    base_frame.z_starting_coord = zero_origin;
  }
  size_t display_width_;
  size_t display_height_;
};