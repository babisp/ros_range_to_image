# ros_range_to_image
A package with nodes that convert range topics to image topics so that a sensor's readings can be visualised with image_view.

Currently, it has only been compiled and tested with ROS Indigo.
## Installation
Include the project in your workspace's .rosinstall with the following lines:
```yaml
- git: {local-name: src/ros_range_to_image,
        uri: 'https://github.com/babisp/ros_range_to_image.git'}
```
Next cd to your workspace and run:
```bash
rosws update
catkin_make
```
## Usage
Launch a node like this:
```bash
rosrun ros_range_to_image laserscan_to_image <laserscan_topic> <image_topic>
# OR
rosrun ros_range_to_image multiecholaserscan_to_image <multiecholaserscan_topic> <image_topic>
```
The sourse range topic must be published by the interface of a sensor. The target image topic will be created by the selected ros_range_to_image node and can be viewed with [image_view](http://wiki.ros.org/image_view).
