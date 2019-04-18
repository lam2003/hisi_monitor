include(ExternalProject)

set(COMPLIE_ARGS "-mcpu=arm926ej-s -w -mno-unaligned-access -fno-aggressive-loop-optimizations -ffunction-sections -fdata-sections")
set(FAST_COMPLIE_OPTION -j4)

#libav
ExternalProject_Add(libav
    URL https://github.com/lam2003/monitor_3rdparty/raw/master/libav-12.3.tar.gz
    DOWNLOAD_NAME libav-12.3.tar.gz
    SOURCE_DIR libav
    CONFIGURE_COMMAND     cd ${PROJECT_BINARY_DIR}/libav && ./configure --arch=arm  --cross-prefix=arm-hisiv500-linux- --enable-static --disable-shared  --enable-cross-compile --target_os=linux --prefix=${PROJECT_BINARY_DIR}/libav/build --extra-cflags=${COMPLIE_ARGS} 
    BUILD_IN_SOURCE 1
    BUILD_COMMAND make ${FAST_COMPLIE_OPTION}
    INSTALL_COMMAND make install && cp ${PROJECT_BINARY_DIR}/libav/build/include ${PROJECT_BINARY_DIR} -rf && cp ${PROJECT_BINARY_DIR}/libav/build/lib ${PROJECT_BINARY_DIR} -rf
    )
#jsoncpp
ExternalProject_Add(jsoncpp
    URL https://github.com/lam2003/monitor_3rdparty/raw/master/jsoncpp-0.10.7.zip
    DOWNLOAD_NAME jsoncpp-0.10.7.zip
    SOURCE_DIR jsoncpp
    CONFIGURE_COMMAND cd ${PROJECT_BINARY_DIR}/jsoncpp && cmake . -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_C_COMPILER=arm-hisiv500-linux-gcc -DCMAKE_CXX_COMPILER=arm-hisiv500-linux-g++ -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/jsoncpp/build
    BUILD_IN_SOURCE 1
    BUILD_COMMAND make ${FAST_COMPLIE_OPTION}
    INSTALL_COMMAND make install && cp ${PROJECT_BINARY_DIR}/jsoncpp/build/include ${PROJECT_BINARY_DIR} -rf && cp ${PROJECT_BINARY_DIR}/jsoncpp/build/lib ${PROJECT_BINARY_DIR} -rf
    )
#easyloggrt
ExternalProject_Add(EasyLogger
    URL https://github.com/lam2003/monitor_3rdparty/raw/master/EasyLogger.tar.gz
    DOWNLOAD_NAME EasyLogger.tar.gz
    SOURCE_DIR EasyLogger
    CONFIGURE_COMMAND cd ${PROJECT_BINARY_DIR}/EasyLogger && cmake . -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_C_COMPILER=arm-hisiv500-linux-gcc -DCMAKE_CXX_COMPILER=arm-hisiv500-linux-g++ -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/EasyLogger/build
    BUILD_IN_SOURCE 1
    BUILD_COMMAND make ${FAST_COMPLIE_OPTION}
    INSTALL_COMMAND make install && cp ${PROJECT_BINARY_DIR}/EasyLogger/build/include ${PROJECT_BINARY_DIR} -rf && cp ${PROJECT_BINARY_DIR}/EasyLogger/build/lib ${PROJECT_BINARY_DIR} -rf
    )
#hisi sdk
ExternalProject_Add(hisi_sdk
    URL https://github.com/lam2003/monitor_3rdparty/raw/master/hisi_sdk.tar.gz
    DOWNLOAD_NAME hisi_sdk.tar.gz
    SOURCE_DIR hisi_sdk
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE 1
    BUILD_COMMAND ""
    INSTALL_COMMAND cp ${PROJECT_BINARY_DIR}/hisi_sdk/include ${PROJECT_BINARY_DIR} -rf && cp ${PROJECT_BINARY_DIR}/hisi_sdk/lib ${PROJECT_BINARY_DIR} -rf
    )
#freetype
ExternalProject_Add(freetype 
    URL https://github.com/lam2003/monitor_3rdparty/raw/master/freetype-2.10.0.tar.gz
    DOWNLOAD_NAME freetype-2.10.0.tar.gz
    SOURCE_DIR freetype
    CONFIGURE_COMMAND cd ${PROJECT_BINARY_DIR}/freetype && ./configure --host=arm-hisiv500-linux --disable-shared --enable-freetype-config --prefix=${PROJECT_BINARY_DIR}/freetype/build CFLAGS=${COMPLIE_ARGS}  CXXFLAGS=${COMPLIE_ARGS}
    BUILD_IN_SOURCE 1
    BUILD_COMMAND make ${FAST_COMPLIE_OPTION}
    INSTALL_COMMAND make install && cp ${PROJECT_BINARY_DIR}/freetype/build/include ${PROJECT_BINARY_DIR} -rf && cp ${PROJECT_BINARY_DIR}/freetype/build/lib ${PROJECT_BINARY_DIR} -rf
    )
#sdl2
ExternalProject_Add(SDL2 
    URL https://github.com/lam2003/monitor_3rdparty/raw/master/SDL2-2.0.9.tar.gz
    DOWNLOAD_NAME SDL2-2.0.9.tar.gz
    SOURCE_DIR SDL2
    CONFIGURE_COMMAND cd ${PROJECT_BINARY_DIR}/SDL2 && ./configure --host=arm-hisiv500-linux --disable-shared --prefix=${PROJECT_BINARY_DIR}/SDL2/build -build=i386 --disable-video-wayland --disable-video-wayland-qt-touch --disable-pulseaudio --disable-video-dummy --disable-arts --disable-esd --disable-alsa --disable-video-x11 --enable-joystick --enable-input-tslib --enable-video-directfb --disable-video-mir  CFLAGS=${COMPLIE_ARGS} CXXFLAGS=${COMPLIE_ARGS}
    BUILD_IN_SOURCE 1
    BUILD_COMMAND make ${FAST_COMPLIE_OPTION}
    INSTALL_COMMAND make install && cp ${PROJECT_BINARY_DIR}/SDL2/build/include ${PROJECT_BINARY_DIR} -rf && cp ${PROJECT_BINARY_DIR}/SDL2/build/lib ${PROJECT_BINARY_DIR} -rf
    )
#sdl2_ttf
ExternalProject_Add(SDL2_ttf
    DEPENDS freetype SDL2
    URL https://github.com/lam2003/monitor_3rdparty/raw/master/SDL2_ttf-2.0.15.tar.gz
    DOWNLOAD_NAME SDL2_ttf-2.0.15.tar.gz
    SOURCE_DIR SDL2_ttf
    CONFIGURE_COMMAND cd ${PROJECT_BINARY_DIR}/SDL2_ttf && export FT2_CONFIG=${PROJECT_BINARY_DIR}/freetype/build/bin/freetype-config && export SDL2_CONFIG=${PROJECT_BINARY_DIR}/SDL2/build/bin/sdl2-config && export PKG_CONFIG_PATH=${PROJECT_BINARY_DIR}/lib/pkgconfig && ./configure --host=arm-hisiv500-linux --prefix=${PROJECT_BINARY_DIR}/SDL2_ttf/build  --disable-shared CFLAGS=${COMPLIE_ARGS}  CXXFLAGS=${COMPLIE_ARGS}
    BUILD_IN_SOURCE 1
    BUILD_COMMAND make ${FAST_COMPLIE_OPTION}
    INSTALL_COMMAND make install && cp ${PROJECT_BINARY_DIR}/SDL2_ttf/build/include ${PROJECT_BINARY_DIR} -rf && cp ${PROJECT_BINARY_DIR}/SDL2_ttf/build/lib ${PROJECT_BINARY_DIR} -rf
    )