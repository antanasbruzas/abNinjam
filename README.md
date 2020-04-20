# abNINJAM

## Building
- `git submodule update --init`
- `mkdir build/ && cd build/ && cmake -DCMAKE_INSTALL_PREFIX:PATH=~/.vst3 ..`
- `cmake --build . --config Release`

## Installing
`cmake --build . --config Release --target install`

## Uninstalling
`cmake --build . --target uninstall`
