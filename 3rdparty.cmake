include(ExternalProject)
#libav
ExternalProject_Add(libav
    URL https://github.com/lam2003/monitor_3rdparty/raw/master/libav-12.3.tar.gz
    DOWNLOAD_NAME libav-12.3.tar.gz
    SOURCE_DIR libav
    CONFIGURE_COMMAND     cd ${PROJECT_BINARY_DIR}/libav && ./configure --arch=arm  --cross-prefix=arm-hisiv500-linux- --enable-static --disable-shared  --enable-cross-compile --target_os=linux --prefix=${PROJECT_BINARY_DIR}/libav/build
    BUILD_IN_SOURCE 1
    BUILD_COMMAND make
    INSTALL_COMMAND make install && cp ${PROJECT_BINARY_DIR}/libav/build/include ${PROJECT_BINARY_DIR} -rf && cp ${PROJECT_BINARY_DIR}/libav/build/lib ${PROJECT_BINARY_DIR} -rf
    )
#jsoncpp
ExternalProject_Add(jsoncpp
    URL https://github.com/lam2003/monitor_3rdparty/raw/master/jsoncpp-0.10.7.zip
    DOWNLOAD_NAME jsoncpp-0.10.7.zip
    SOURCE_DIR jsoncpp
    CONFIGURE_COMMAND cd ${PROJECT_BINARY_DIR}/jsoncpp && cmake . -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_C_COMPILER=arm-hisiv500-linux-gcc -DCMAKE_CXX_COMPILER=arm-hisiv500-linux-g++ -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/jsoncpp/build
    BUILD_IN_SOURCE 1
    BUILD_COMMAND make
    INSTALL_COMMAND make install && cp ${PROJECT_BINARY_DIR}/jsoncpp/build/include ${PROJECT_BINARY_DIR} -rf && cp ${PROJECT_BINARY_DIR}/jsoncpp/build/lib ${PROJECT_BINARY_DIR}/lib/ -rf
    )
