# Compiling obs-websocket-avt

## Prerequisites

You'll need [Qt 6.3.1](https://download.qt.io/official_releases/qt/6.3/6.3.1/),
[CMake](https://cmake.org/download/) and a working [OBS Studio development environment](https://obsproject.com/wiki/install-instructions) installed on your
computer.

Read [Building OBS Studio](https://obsproject.com/wiki/install-instructions#building-obs-studio) to setup your environment.

## Windows

Put all changes into plugins/obs-websocket and rebuilt the project.
You will get the binaries in `build64/rundir`
- obs-plugins/obs-websocket-avt.dll
- data/obs-plugins/obs-websocket-avt

You may encounter some build errors about incorrect unicode format files. Open those error files and save it as UTF-8 with BOM.


