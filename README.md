# abNINJAM

## Build
- Initialize dependencies:
`git submodule update --init`
- Configure build:
`mkdir build/ && cd build/ && cmake ..`
To set custom installation directory instead use:
`mkdir build/ && cd build/ && cmake -DVSTPLUGIN_INSTALL_DIR:PATH=/your/custom/path ..`
- Compile:
`cmake --build .`

## Install
`cmake --build . --target install`

## Uninstall
`cmake --build . --target uninstall`
