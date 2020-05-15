# abNINJAM

## Building from source

### Linux (Ubuntu)

#### Build
- Install dependencies:  
`sudo apt-get install libxcb-util-dev libxcb-cursor-dev libxcb-keysyms1-dev libxcb-xkb-dev libxkbcommon-dev libxkbcommon-x11-dev libvorbis-dev zenity lv2-dev`
- Initialize submodule dependencies:  
`git submodule update --init`
- Configure build:  
`mkdir build/ && cd build/ && cmake ..`
- To set custom installation directory use `mkdir build/ && cd build/ && cmake -DVSTPLUGIN_INSTALL_DIR:PATH=/your/custom/path ..` instead
- To build without GUI use option `-DWITHOUT_GUI=ON`
- To build with logging to file use option `-DLOG_FILE=ON`
- Compile:  
`cmake --build .`

#### Install
`cmake --build . --target install`

#### Uninstall
`cmake --build . --target uninstall`


### Windows

#### Build
- Install dependencies:  
Download `https://github.com/maravento/winzenity/raw/master/zenity.zip` and extract to `%HOMEDRIVE%\%HOMEPATH%\abNninjam\`  
`vcpkg install libvorbis:x64-windows-static`  
`vcpkg integrate install`
- Initialize submodule dependencies:  
`git submodule update --init`
- Configure build:  
`mkdir build/ && cd build/ && cmake -DCMAKE_CONFIGURATION_TYPES="Release" -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_TOOLCHAIN_FILE=/your/toolchain/file.cmake ..`
- To build without GUI use option `-DWITHOUT_GUI=ON`
- To build with logging to file use option `-DLOG_FILE=ON`
- Compile:  
`cmake --build . --config Release`

#### Install
`cmake --build . --target install --config Release`

### MacOS

#### Build
- Install dependencies:  
`brew install pkg-config`  
`brew install libvorbis`  
`brew install zenity`
`brew install lv2`
- Initialize submodule dependencies:  
`git submodule update --init`
- Configure build:  
`mkdir build/ && cd build/ && cmake -DOGG_INCLUDE_DIRS=/usr/local/Cellar/libogg/1.3.4/include -DOGG_LIBRARIES=/usr/local/Cellar/libogg/1.3.4/lib/libogg.a -DVORBIS_INCLUDE_DIRS=/usr/local/Cellar/libvorbis/1.3.6/include -DVORBIS_LIBRARIES=/usr/local/Cellar/libvorbis/1.3.6/lib/libvorbis.a -DVORBISENC_INCLUDE_DIRS=/usr/local/Cellar/libvorbis/1.3.6/include -DVORBISENC_LIBRARIES=/usr/local/Cellar/libvorbis/1.3.6/lib/libvorbisenc.a -GXcode ..`
- To build without GUI use option `-DWITHOUT_GUI=ON`
- To build with logging to file use option `-DLOG_FILE=ON`
- Compile:  
`cmake --build . --config Release`

#### Install
`cmake --build . --target install --config Release`

## Usage
Plugin can be used with GUI or specifying parameters in `~/abNinjam/connection.properties` file. Available options: 
- `host=192.168.0.145`
- `user=antanas`
- `pass=supersecret`
- `autoLicenseAgree=true`
- `autoRemoteVolume=true`

autoLicenseAgree property is used to automatically agree to the license provided by the server (for example if you use your own server and know the license) (default: false).  
autoRemoteVolume is set for adjusting remote channel volume by distributing it and protecting from clipping (default: true).
