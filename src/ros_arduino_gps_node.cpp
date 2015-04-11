#include <iostream>
#include <stdio.h>

#include "ros/ros.h"
#include "sensor_msgs/NavSatFix.h"

ros::Publisher gpsPublisher;

#include "serial/serial.h"

int main(int argc, char **argv)
{
	ROS_INFO("Starting ros_arduino_gps_node");

	ros::init(argc, argv, "ros_arduino_gps");
	ros::NodeHandle node;

	ros::Publisher gpsPublisher = node.advertise<sensor_msgs::NavSatFix>("/gps", 1);

	// port, baudrate, timeout in milliseconds
	serial::Serial gpsSerial("/dev/ttyACM0", 115200, serial::Timeout::simpleTimeout(1000));

	if(gpsSerial.isOpen())
	{
		ROS_INFO("Successfully connected to GPS Arduino.");
	}
	else
	{
		ROS_INFO("Shit. Failed to connect.");
	}

	ROS_INFO("Press Ctrl-C to kill node.");

	// Spin
	ros::Rate loopRate(1); // 1 hz

	while(ros::ok())
	{
		// Send GPS data message
		sensor_msgs::NavSatFix msg;

		// Data Index
		gpsSerial.readline(20, " ");

		// Latitude
		std::stringstream ssLat(gpsSerial.readline(20, " "));
		int latitude;
		ssLat >> latitude;
		msg.latitude = latitude;

		// Longitude
		std::stringstream ssLong(gpsSerial.readline(20, " "));
		float longitude;
		ssLong >> longitude;
		msg.longitude = longitude;

		// Altitude
		std::stringstream ssAlt(gpsSerial.readline(20, " "));
		float altitude;
		ssAlt >> altitude;
		msg.altitude = altitude;

		// Publish the message
		gpsPublisher.publish(msg);

		// Clear rest of the line
		gpsSerial.readline();

		// ROS Spin & Sleep
		ros::spinOnce();
		loopRate.sleep();
	}

	ros::shutdown();

	return 0;
}