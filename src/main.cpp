

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

void testdrawline(Arduipi_NS::ArduiPi_OLED& display) {  
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, WHITE);
    display.display();
  }
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, WHITE);
    display.display();
  }
  delay(1000);
  
  display.clearDisplay();
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, display.height()-1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(0, display.height()-1, display.width()-1, i, WHITE);
    display.display();
  }
  delay(1000);
  
  display.clearDisplay();
  for (int16_t i=display.width()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, 0, i, WHITE);
    display.display();
  }
  delay(1000);

  display.clearDisplay();
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, WHITE);
    display.display();
  }
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, WHITE); 
    display.display();
  }
  delay(1000);
}

int main() {

  struct sigaction sig_int_handler;
  setup_sigint_handler(sig_int_handler);

  // Instantiate the display
  Arduipi_NS::ArduiPi_OLED display;
  // I2C change parameters to fit to your LCD
  if (!display.init(OLED_I2C_RESET, OLED_ADAFRUIT_I2C_128x64))
    exit(EXIT_FAILURE);
  display.begin();
  // init done
  display.clearDisplay();   // clears the screen  buffer
  display.display();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Hello, world!\n");
  display.display();
  std::cout << "showing hi" << std::endl;
  delay(2000);

  size_t width = 128;
  size_t height = 64;
  IMUVisualizer visualizer(width, height);

  double rotation = 0;

  while (!EXIT_FLAG) {
    display.clearDisplay();

    testdrawline(display);

 /*   CoordinateFrame new_frame =
        visualizer.get_rotated_coordinate_frame(0, 0, rotation);
    std::cout << "original coord\n" << new_frame.frame << std::endl;
    FlattenedCoordinateFrame flattened_frame =  visualizer.flatten_coordinate_frame(new_frame);

    // std::cout << flattened_frame.frame << std::endl;
    int r = static_cast<int>(flattened_frame.frame.rows());

    for (int i = 1; i < r; i += 2) {
      std::cout << "drawing line " << i << std::endl;
      int16_t startx = flattened_frame.frame(i - 1, 0);
      int16_t starty = flattened_frame.frame(i-1, 1);
      int16_t endx = flattened_frame.frame(i, 0);
      int16_t endy = flattened_frame.frame(i, 1);

      if (endx > width)
      {
        endx = width - 1;
      }
      if (endx < 0)
      {
        endx = 0;
      }

      if (endy > height)
      {
        endy = height - 1;
      }

      if (endy < 0)
      {
        endy = 0;
      }

      std::cout << "  from " << startx << "," << starty << " to " << endx << "," << endy << std::endl;    
      display.drawLine(
          startx, starty, endx, endy, 1);
    }

    std::cout << " hi" << std::endl;
    display.display();
    
    */

    rotation += 1 * M_PI / 180;
    delay(LOOP_DELAY_MS);
  }

  display.clearDisplay();
  display.display();
  display.close();
}

