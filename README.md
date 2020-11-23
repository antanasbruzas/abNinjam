# abNinjam

## Building from source

To disable vst3 build use `-DABNINJAM_VST=OFF`  
To disable lv2 build use `-DABNINJAM_LV2=OFF`  

### Linux (Ubuntu)

#### Build
- Install dependencies:  
`sudo apt-get install libxcb-util-dev libxcb-cursor-dev libxcb-keysyms1-dev libxcb-xkb-dev libxkbcommon-dev libxkbcommon-x11-dev libvorbis-dev zenity lv2-dev libfreetype-dev libcairo2-dev liblo-dev`
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
`brew install liblo`
- Initialize submodule dependencies:  
`git submodule update --init`
- Configure build:  
`mkdir build/ && cd build/ && cmake -DCMAKE_BUILD_TYPE=Release -DOGG_ROOT=/usr/local/Cellar/libogg/ -DVorbis_ROOT=/usr/local/Cellar/libvorbis/ -DLIBLO_ROOT=/usr/local/Cellar/liblo/ -DLV2_ROOT=/usr/local/Cellar/lv2/ -GXcode ..`
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
- `autoSyncBpm=true`

Leaving `pass` empty will connect anonymously (if the server permits) and `user` is used as the nickname.
autoLicenseAgree property is used to automatically agree to the license provided by the server (for example if you use your own server and know the license) (default: false).  
autoRemoteVolume is set for adjusting remote channel volume by distributing it and protecting from clipping (default: true).  
autoSyncBpm enables sending OSC message `/tempo/raw {int}` to host to change it's BPM if BPM is changed on Ninjam server. As well as sending /bpm command or voting command to change BPM for Ninjam server if BPM is changed on host. (default: true)
