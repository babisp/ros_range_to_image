#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <stdint.h>

//ROS
#include <ros/ros.h>
#include <ros/console.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>

void subscriberCallback(const sensor_msgs::LaserScan::ConstPtr& scan);

ros::Publisher publisher;

int main(int argc, char** argv)
{

	// check the arguments
	if (argc < 3)
	{
		std::cerr << "USAGE: " << argv[0] << " <source LaserScan topic> <target Image topic>" << std::endl;
		return 0;
	}

	std::string sourceTopic(argv[1]), destTopic(argv[2]);

	ROS_INFO_STREAM("Translating laserscans from topic \"" << sourceTopic << "\" to images in topic \"" << destTopic << "\"");

	ros::init(argc, argv, "laserscan_to_image");
	ros::NodeHandle nh;

	publisher = nh.advertise<sensor_msgs::Image>(destTopic, 1000);
	ros::Subscriber subscriber = nh.subscribe(sourceTopic, 1000, subscriberCallback);

	ros::spin();

	return 0;

}

void subscriberCallback(const sensor_msgs::LaserScan::ConstPtr& scan)
{
	float rangeMin = scan->range_min,
	      rangeMax = scan->range_max,
	      diff = rangeMax - rangeMin,
	      angleMin = scan->angle_min,
	      angleMax = scan->angle_max,
	      angleIncr = scan->angle_increment;
	int num = (int) ((angleMax - angleMin) / angleIncr);
	int height = num / (fabs(sin(angleMin)) + fabs(sin(angleMax)));

	float mRanges[num];
	float theta = angleMin;
	for (int i = 0; i < num; i++)
	{
		mRanges[i] = (scan->ranges[i] - rangeMin) / diff * cos(theta);
		theta += angleIncr;
	}

	// publish the image
	sensor_msgs::Image image;
	image.header = scan->header;
	image.height = height;
	image.width = num;
	image.encoding = sensor_msgs::image_encodings::MONO8;
	image.step = sizeof(unsigned char) * num;

	image.data.resize(num * height);
	for (int i = 0; i < height; i++)
		for (int j = 0; j < num; j++)
			image.data[num * i + j] = (i / (float) height < mRanges[j] || scan->ranges[j] == rangeMax) ? 0 : 255;

	publisher.publish(image);

}
