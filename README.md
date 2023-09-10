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

### Linux
For linux you will have to use OpenNI-Linux-x64-2.3 library. All you need is to download it and extract it into `external/` folder. When the previous step is complete follow the next one.

2. Launch Qt Creator and choose "Open Project", then open CMakeLists.txt and press ctrl + r. After done that, wait for a couple of minutes while the project is being built and run. It is the right time to get a cup of coffee made.


