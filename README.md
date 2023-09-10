# onip 

## description

onip - is nothing but a tiny `*.oni` file player. Currently, onip supports color and z-buffer image rendering.

## prerequesities

In order to build and run onip you have to have installed following on you system:

* Qt 5.12.2
* Qt Creator

## installation

### Common steps

1. Clone the repo and intialize submodules:

```
git clone git@github.com:che6a/onip.git
cd onip
git submodule init
git submodule update
```
2. Launch Qt Creator and choose "Open Project", then open CMakeLists.txt and press ctrl + r. After doing that, wait for a couple of minutes while the project is being built and run. It is the right time to get a cup of coffee made.

### Linux
For linux you will have to use [OpenNI-Linux-x64-2.3](https://github.com/choonyip/OpenNI-Linux-x64-2.3) library. All you need is to download it and extract it into `external/` folder.

## Basic onip usage and results of the work:

When you see onip window appeared, click on "file" in the menubar or simply press ctrl + o to open an `.oni` file.
Press space or play/pause button in order to start / stop the video.
In order to change color image with z-buffer, press 1 and 2 correspondingly.

### What has been implemented:
- Open `.oni` file through the `QFileDialog`.
- Color and z-buffer image rendering.
- You can play and pause the video.
- You can choose an arbitrary frame using the timeline.

### What has not been:
- Seek forward and rewind by frame.
- Sobel operator.
