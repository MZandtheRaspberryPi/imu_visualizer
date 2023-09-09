

#include <csignal>
#include <iostream>

// #include "Adafruit_GFX.h"
#include "ArduiPi_OLED.h"
// #include "ArduiPi_OLED_lib.h"
#include "visualizer.h"

bool EXIT_FLAG = false;
const uint16_t LOOP_DELAY_MS = 10;

void my_signal_handler(int s) {
  printf("Caught signal %d\n", s);
  EXIT_FLAG = true;
}

void setup_sigint_handler(struct sigaction &sig_int_handler) {
  sig_int_handler.sa_handler = my_signal_handler;
  sigemptyset(&sig_int_handler.sa_mask);
  sig_int_handler.sa_flags = 0;
  sigaction(SIGINT, &sig_int_handler, NULL);
}

int main() {

  struct sigaction sig_int_handler;
  setup_sigint_handler(sig_int_handler);

  // Instantiate the display
  Arduipi_NS::ArduiPi_OLED display;
  // I2C change parameters to fit to your LCD
  if (!display.init(OLED_I2C_RESET, OLED_ADAFRUIT_I2C_128x64))
    exit(EXIT_FAILURE);
  std::cout << "hi" << std::endl;
  size_t width = 128;
  size_t height = 64;
  IMUVisualizer visualizer(width, height);

  double rotation = 0;

  while (!EXIT_FLAG) {

    CoordinateFrame new_frame =
        visualizer.get_rotated_coordinate_frame(0, 0, rotation);
    std::cout << "original coord\n" << new_frame.frame << std::endl;
    FlattenedCoordinateFrame flattened_frame =
        visualizer.flatten_coordinate_frame(new_frame);
    std::cout << flattened_frame.frame << std::endl;

    int r = static_cast<int>(flattened_frame.frame.rows());

    for (int i = 1; i < r; r += 2) {
      display.drawLine(
          flattened_frame.frame(i - 1, 0), flattened_frame.frame(i - 1, 1),
          flattened_frame.frame(i, 0), flattened_frame.frame(i, 1), 1);
    }
    display.display();

    rotation += 1 * M_PI / 180;
    delay(LOOP_DELAY_MS);
  }

  display.clearDisplay();
  display.display();
}