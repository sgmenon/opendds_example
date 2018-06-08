# :goat: opendds-example

## Overview

## Prerequisites
You need to have `OpenDDS` built, and your environment setup accordingly

## Building on Windows

Set up your environment. Here's what you'll need:

1. Set `$DDS_ROOT` to wherever you installed your OpenDDS binaries
2. Set `$TAO_ROOT` to `$DDS_ROOT/ACE_wrappers/TAO`
3. Set `$ACE_ROOT` to `$DDS_ROOT/ACE_wrappers/MPC`
4. Make sure that `$ACE_ROOT/bin;$DDS_ROOT/bin` are on your system path
5. Make sure that `$ACE_ROOT/lib;$DDS_ROOT/lib` are on your library search path (same as system path on Windows)

To build you need to run the following commands
```sh
$ cd build_win64
$ cmake -G "Visual Studio 15 2017 Win64" ../
$ opendds_example.sln
```

you could also simply build by typing the following command in the build directory
```sh
$ cmake --build . --config Release
```

## Running an executable on Windows

CD to the directory where your executable is
```bash
$ %DDS_ROOT%/bin/DCPSInfoRepo -NOBITS -ORBListenEndpoints iiop://localhost:12345 &
$ StockQuoter_Publisher.exe -DSPCBit 0 -DCPSConfigFile %PROJ_ROOT%/config/dds_tcp_conf.ini &
$ StockQuoter_Subscriber.exe -DSPCBit 0 -DCPSConfigFile %PROJ_ROOT%/config/dds_tcp_conf.ini
```

where `$PROJ_ROOT` is the root directory of this GIT project.


## Building and Running on Linux
Just use `Docker` for all your builds!

A `Docker` image containing a pre-built OpenDDS is available on docker-hub.


0. If `Docker` is not installed, you may simply type:

        apt-get install docker

1. Check for prerequisites

        docker --version

        docker-compose --version

2. Enter a container

        docker build -t opendds_example_img .

        docker run --user $UID --rm -ti -v "$PWD:/opt/OpenDDS_Example" opendds_example_img

3. Build the example - I don't know why, but #include "orbsvc/TimeBaseC.h" does not seem to get generated in the IDL soruce files.

        cd /opt/OpenDDS_Example
        cd build_glnxa64
        cmake ../
        make CXX_FLAGS="-include orbsvcs/TimeBaseC.h -std=c++14"  


4. Exit the container

        exit

5. Run the DDS example with RTPS

        docker-compose up



## Authors

Siddharth Menon
:heart:
