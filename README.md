# Rosbag IO

This library allows the rosbag files manipulation (reading and writing) without any ROS dependencies.

It has been adapted from the package [rosbag_storage](http://wiki.ros.org/rosbag_storage).

This library does not support rosbag encryption and only support the rosbag byte array message: `UInt8MultiArray`.

## Installation

In order to install the library, the following commands have to be executed:

```
mkdir build && cd build
cmake .. && make
make install
```

## Distribute the library

In order to share the library via a `deb` package the following commands can be executed:

```
mkdir build && cd build
cmake .. && make
cpack
```
