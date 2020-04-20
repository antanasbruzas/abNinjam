# abNINJAM

## Building
- `git submodule update --init1`
- `git submodule sync`
- `mkdir build/ && cd build/ && cmake ..`
- `cmake -DCMAKE_INSTALL_PREFIX=~/.vst3 --build . --config Release`

## Installing
`cmake --build . --config Release --target install`

## Uninstalling
`cmake --build . --target uninstall`
