# abNINJAM

## Build
- Initialize dependencies:
`git submodule update --init`
- Configure build:
`mkdir build/ && cd build/ && cmake -DCMAKE_INSTALL_PREFIX:PATH=~/.vst3 ..`
- Compile:
`cmake --build . --config Release`

## Install
`cmake --build . --config Release --target install`

## Uninstall
`cmake --build . --target uninstall`
