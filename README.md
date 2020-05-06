# abNINJAM

## Build
- Initialize dependencies:
`git submodule update --init`
- Configure build:
`mkdir build/ && cd build/ && cmake ..`  
- To set custom installation directory use `mkdir build/ && cd build/ && cmake -DVSTPLUGIN_INSTALL_DIR:PATH=/your/custom/path ..` instead
- To build without GUI use option `-DWITHOUT_GUI=ON`


- Compile:
`cmake --build .`

## Install
`cmake --build . --target install`

## Uninstall
`cmake --build . --target uninstall`

## Usage
Plugin can be used with GUI or specifying parameters in `~/abNinjam/connection.properties` file. Available options: 
- `host=192.168.0.145`
- `user=antanas`
- `pass=supersecret`
- `licenseAutoAgree=true`

licenseAutoAgree property is used to automatically agree to the license provided by the server (for example if you use your own server and know the license)
