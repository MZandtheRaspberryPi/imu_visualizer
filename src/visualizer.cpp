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
    triad.y = msg.ground_truth().xyz().y();
    triad.z = msg.ground_truth().xyz().z();
    quat.xyz = triad;
    msg_vis.ground_truth = quat;
  }

  TriadVis triad{};
  if (msg.has_euler_angles()) {
    triad.x = msg.euler_angles().x();
    triad.y = msg.euler_angles().y();
    triad.z = msg.euler_angles().z();
    msg_vis.euler_angles = triad;
  }

  if (msg.has_linear_acceleration()) {
    triad.x = msg.linear_acceleration().x();
    triad.y = msg.linear_acceleration().y();
    triad.z = msg.linear_acceleration().z();
    msg_vis.linear_acceleration = triad;
  }

  if (msg.has_angular_acceleration()) {
    triad.x = msg.angular_acceleration().x();
    triad.y = msg.angular_acceleration().y();
    triad.z = msg.angular_acceleration().z();
    msg_vis.angular_acceleration = triad;
  }

  if (msg.has_magnetometer_vector()) {
    triad.x = msg.magnetometer_vector().x();
    triad.y = msg.magnetometer_vector().y();
    triad.z = msg.magnetometer_vector().z();
    msg_vis.magnetometer_vector = triad;
  }

  if (msg.has_board_temp()) {
    msg_vis.board_temp = msg.board_temp();
  }
  if (msg.has_system_calibration()) {
    msg_vis.system_calibration = msg.system_calibration();
  }
  if (msg.has_gyro_calibration()) {
    msg_vis.gyro_calibration = msg.gyro_calibration();
  }
  if (msg.has_accel_calibration()) {
    msg_vis.accel_calibration = msg.accel_calibration();
  }
  if (msg.has_mag_calibration()) {
    msg_vis.mag_calibration = msg.mag_calibration();
  }
  if (msg.has_filter_timestamp()) {
    msg_vis.filter_timestamp = msg.filter_timestamp();
  }

  if (msg.has_euler_angles_filter()) {
    triad.x = msg.euler_angles_filter().x();
    triad.y = msg.euler_angles_filter().y();
    triad.z = msg.euler_angles_filter().z();
    msg_vis.euler_angles_filter = triad;
  }

  if (msg.has_cov_matrix_filter()) {
    const imu_msgs::CovarianceMatrix &cov_matrix = msg.cov_matrix_filter();
    size_t row_counter = 0;
    double trace = 0.0;
    for (const imu_msgs::MatrixRow &row : cov_matrix.row()) {
      size_t within_row_ctr = 0;
      for (const double &val : row.val()) {
        if (row_counter == within_row_ctr) {
          trace += val;
          break;
        }
        within_row_ctr++;
      }
      row_counter++;
    }

    msg_vis.covariance_matrix_trace = trace;
  }
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
  // std::cout << "original coord\n" << new_frame.frame << std::endl;
  FlattenedCoordinateFrame flattened_frame =
      vis.flatten_coordinate_frame(new_frame);

  // std::cout << "flattened coord\n" << flattened_frame.frame << std::endl;

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

  ImuMsgVis msg_vis{};
  if (listener->has_msg()) {
    imu_msgs::ImuMsg imu_msg = listener->get_msg();
    proto_msg_to_c_struct(imu_msg, msg_vis);
  } else {
    msg_vis.has_msg = false;
  }

  return msg_vis;
}
}
