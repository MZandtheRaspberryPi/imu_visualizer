#include "visualizer.h"

void proto_msg_to_c_struct(const imu_msgs::ImuMsg &msg, ImuMsgVis &msg_vis) {

  msg_vis.has_msg = true;

  if (msg.has_timestamp()) {
    msg_vis.timestamp = msg.timestamp();
  }

  if (msg.has_ground_truth()) {
    QuaternionVis quat;
    quat.w = msg.ground_truth().w();

    TriadVis triad;
    triad.x = msg.ground_truth().xyz().x();
    triad.y = msg.ground_truth().xyz().x();
    triad.z = msg.ground_truth().xyz().x();
    quat.xyz = triad;
    msg_vis.ground_truth = quat;
  }

  if (msg.has_euler_angles())
  {
    TriadVis triad;
    triad.x = msg.euler_angles().x();
    triad.y = msg.euler_angles().y();
    triad.z = msg.euler_angles().z();
    msg_vis.euler_angles = triad;
  }

  /*

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
  */
}

std::shared_ptr<ListenerClient> get_listener() {
  static std::shared_ptr<ListenerClient> listener_ptr(nullptr);
  if (listener_ptr == nullptr) {
    listener_ptr = std::shared_ptr<ListenerClient>(new ListenerClient());
  }
  return listener_ptr;
}

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

  std::cout << "flattened coord\n" << flattened_frame.frame << std::endl;

  FlattenedCoordinateFrameNonMatrix flatted_frame_non_mat;
  flatted_frame_non_mat.x_start_x = flattened_frame.frame(0, 0);
  flatted_frame_non_mat.x_start_y = flattened_frame.frame(0, 1);
  flatted_frame_non_mat.x_end_x = flattened_frame.frame(1, 0);
  flatted_frame_non_mat.x_end_y = flattened_frame.frame(1, 1);
  flatted_frame_non_mat.y_start_x = flattened_frame.frame(2, 0);
  flatted_frame_non_mat.y_start_y = flattened_frame.frame(2, 1);
  flatted_frame_non_mat.y_end_x = flattened_frame.frame(3, 0);
  flatted_frame_non_mat.y_end_y = flattened_frame.frame(3, 1);
  flatted_frame_non_mat.z_start_x = flattened_frame.frame(4, 0);
  flatted_frame_non_mat.z_start_y = flattened_frame.frame(4, 1);
  flatted_frame_non_mat.z_end_x = flattened_frame.frame(5, 0);
  flatted_frame_non_mat.z_end_y = flattened_frame.frame(5, 1);
  return flatted_frame_non_mat;
}

void start_listening() {
  static std::shared_ptr<ListenerClient> listener_ptr = get_listener();
  std::string ip_address = "filter";
  uint16_t port = 9001;
  listener_ptr->connect(ip_address, port);
  listener_ptr->start_non_blocking();
}

void stop_listening() {
  std::shared_ptr<ListenerClient> listener = get_listener();
  listener->stop();
}

ImuMsgVis get_latest_imu_msg() {

  std::shared_ptr<ListenerClient> listener = get_listener();

  ImuMsgVis msg_vis;
  if (listener->has_msg()) {
    imu_msgs::ImuMsg imu_msg = listener->get_msg();
    proto_msg_to_c_struct(imu_msg, msg_vis);
  } else {
    msg_vis.has_msg = false;
  }

  return msg_vis;
}
}
