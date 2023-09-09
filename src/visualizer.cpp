#include "visualizer.h"

extern "C" {
FlattenedCoordinateFrameNonMatrix
rotate_frame(double x_rotation, double y_rotation, double z_rotation) {

  static double width = 128;
  static double height = 64;
  static IMUVisualizer vis(width, height);

  CoordinateFrame new_frame =
      vis.get_rotated_coordinate_frame(x_rotation, y_rotation, z_rotation);
  std::cout << "original coord\n" << new_frame.frame << std::endl;
  FlattenedCoordinateFrame flattened_frame =
      vis.flatten_coordinate_frame(new_frame);

  FlattenedCoordinateFrameNonMatrix flatted_frame_non_mat;
  flatted_frame_non_mat.x_start_x = flattened_frame.frame(0, 0);
  flatted_frame_non_mat.x_start_y = flattened_frame.frame(0, 1);
  flatted_frame_non_mat.x_end_x = flattened_frame.frame(1, 0);
  flatted_frame_non_mat.x_end_y = flattened_frame.frame(1, 1);
  flatted_frame_non_mat.y_start_x = flattened_frame.frame(0, 0);
  flatted_frame_non_mat.y_start_y = flattened_frame.frame(0, 1);
  flatted_frame_non_mat.y_end_x = flattened_frame.frame(1, 0);
  flatted_frame_non_mat.y_end_y = flattened_frame.frame(1, 1);
  flatted_frame_non_mat.z_start_x = flattened_frame.frame(0, 0);
  flatted_frame_non_mat.z_start_y = flattened_frame.frame(0, 1);
  flatted_frame_non_mat.z_end_x = flattened_frame.frame(1, 0);
  flatted_frame_non_mat.z_end_y = flattened_frame.frame(1, 1);
  return flatted_frame_non_mat;
}
}
