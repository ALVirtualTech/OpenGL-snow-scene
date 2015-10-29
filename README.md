OpenGL snow scene
=================

Yet another project for a graphics programming course. It utilizes
amongst other things geometry instancing to speed up rendering.


Requirements
------------

You need to have a computer with a graphics card capable of running
OpenGL 3.2 or later to run this demo. In addition it has only been
tested on Linux, CentOS 6 and Ubuntu 14.04 specifically. In order to
install the requiered libraries on Ubuntu, run the following command:

```
sudo apt-get install freeglut3-dev
```
In addition you need to install GL.h, which is located in different
packages depending on your graphics card vendor and driver of choice.


Installation
------------

The following instructions will set up and run the project:

```
git clone git@github.com:Rovanion/OpenGL-Geometry-Instancing.git
cd OpenGL-Geometry-Instancing/build
cmake ..
make
./Project
```
