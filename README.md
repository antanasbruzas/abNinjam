# abNINJAM

## Building from source

### Linux (Ubuntu)

#### Build
- Install dependencies:
`sudo apt-get install libxcb-util-dev libxcb-cursor-dev libxcb-keysyms1-dev libxcb-xkb-dev libxkbcommon-dev libxkbcommon-x11-dev libvorbis-dev`
- Initialize submodule dependencies:
`git submodule update --init`
- Configure build:
`mkdir build/ && cd build/ && cmake ..`  
- To set custom installation directory use `mkdir build/ && cd build/ && cmake -DVSTPLUGIN_INSTALL_DIR:PATH=/your/custom/path ..` instead
- To build without GUI use option `-DWITHOUT_GUI=ON`
- Compile:
`cmake --build .`

#### Install
`cmake --build . --target install`

#### Uninstall
`cmake --build . --target uninstall`


### Windows

#### Build
- Install dependencies:
`vcpkg install libvorbis:x64-windows-static`
`vcpkg integrate install`
- Initialize submodule dependencies:
`git submodule update --init`
- Configure build:
`mkdir build/ && cd build/ && cmake -DCMAKE_CONFIGURATION_TYPES="Release" -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_TOOLCHAIN_FILE=/your/toolchain/file.cmake ..`  
- To build without GUI use option `-DWITHOUT_GUI=ON`
- Compile:
`cmake --build . --config Release`

#### Install
`cmake --build . --target install --config Release`

## Usage
Plugin can be used with GUI or specifying parameters in `~/abNinjam/connection.properties` file. Available options: 
- `host=192.168.0.145`
- `user=antanas`
- `pass=supersecret`
- `licenseAutoAgree=true`

licenseAutoAgree property is used to automatically agree to the license provided by the server (for example if you use your own server and know the license)
