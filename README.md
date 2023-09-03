# imu_visualizer
a repo to read imu messages from a filter via websockets, serialized with protobuffer, and visualize the orientation on a pixel grid, outputting to an oled via i2c.


```
sudo docker run -it --device /dev/i2c-3 --device /dev/mem --privileged --entrypoint=/bin/bash oled
```
