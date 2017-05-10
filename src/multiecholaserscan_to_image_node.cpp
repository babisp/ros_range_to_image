#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <stdint.h>

//ROS
#include <ros/ros.h>
#include <ros/console.h>
#include <sensor_msgs/MultiEchoLaserScan.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>

void subscriberCallback(const sensor_msgs::MultiEchoLaserScan::ConstPtr& scan);

ros::Publisher publisher;
std::string destTopic;

int main(int argc, char** argv)
{

  // check the arguments
  if (argc < 3)
  {
    std::cerr << "USAGE: " << argv[0] << " <source MultiEchoLaserScan topic> <target Image topic>" << std::endl;
    return 0;
  }

  std::string sourceTopic(argv[1]);
  destTopic = std::string(argv[2]);

  ROS_INFO_STREAM("Translating MultiEchoLaserScans from topic \"" << sourceTopic << "\" to images in topic \"" << destTopic << "\"");

  ros::init(argc, argv, "MultiEchoLaserScan_to_image");
  ros::NodeHandle nh;

  publisher = nh.advertise<sensor_msgs::Image>(destTopic, 1000);
  ros::Subscriber subscriber = nh.subscribe(sourceTopic, 1000, subscriberCallback);

  ros::spin();

  return 0;

}

void subscriberCallback(const sensor_msgs::MultiEchoLaserScan::ConstPtr& scan)
{
  float rangeMin = scan->range_min,
        rangeMax = scan->range_max,
        diff = rangeMax - rangeMin,
        angleMin = scan->angle_min,
        angleMax = scan->angle_max,
        angleIncr = scan->angle_increment;
  int width = (int) scan->ranges.size();
  if (!width)
  {
    ROS_WARN_STREAM("The incoming messages from topic \"" << destTopic << "\" do not contain any sensor readings.");
    return;
  }
  int height = (int) scan->ranges[0].echoes.size();
  if (!height)
  {
    ROS_WARN_STREAM("The incoming messages from topic \"" << destTopic << "\" do not contain any sensor readings.");
    return;
  }

  float mRanges[width][height];
  // float theta = angleMin;
  for (int i = 0; i < width; i++)
    for (int j = 0; j < height; j++)
    {
      mRanges[i][j] = (scan->ranges[i].echoes[j] - rangeMin) / diff;
      // mRanges[i][j] = (scan->ranges[i].echoes[j] - rangeMin) / diff * cos(theta);
      // theta += angleIncr;
    }

  // publish the image
  sensor_msgs::Image image;
  image.header = scan->header;
  image.width = width;
  image.height = height;
  image.encoding = sensor_msgs::image_encodings::MONO8;
  image.step = sizeof(unsigned char) * width;

  image.data.resize(width * height);
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
      image.data[width * i + j] = (1.0 - mRanges[i][j]) * 255; // the closer the point, the brighter the pixel

  publisher.publish(image);

}
