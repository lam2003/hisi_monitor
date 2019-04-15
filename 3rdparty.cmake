include(ExternalProject)

set(USE_FAST_COMPLIE true)
set(CROSS_COMPLIE_OPTION -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_C_COMPILER=arm-hisiv500-linux-gcc -DCMAKE_CXX_COMPILER=arm-hisiv500-linux-g++)

if(USE_FAST_COMPLIE)
set(FAST_COMPLIE_OPTION -j4)
endif()

#libav
ExternalProject_Add(libav
    URL https://github.com/lam2003/monitor_3rdparty/raw/master/libav-12.3.tar.gz
    DOWNLOAD_NAME libav-12.3.tar.gz
    SOURCE_DIR libav
    CONFIGURE_COMMAND     cd ${PROJECT_BINARY_DIR}/libav && ./configure --arch=arm  --cross-prefix=arm-hisiv500-linux- --enable-static --disable-shared  --enable-cross-compile --target_os=linux --prefix=${PROJECT_BINARY_DIR}/libav/build
    BUILD_IN_SOURCE 1
    BUILD_COMMAND make ${FAST_COMPLIE_OPTION}
    INSTALL_COMMAND make install && cp ${PROJECT_BINARY_DIR}/libav/build/include ${PROJECT_BINARY_DIR} -rf && cp ${PROJECT_BINARY_DIR}/libav/build/lib ${PROJECT_BINARY_DIR} -rf
    )
#jsoncpp
ExternalProject_Add(jsoncpp
    URL https://github.com/lam2003/monitor_3rdparty/raw/master/jsoncpp-0.10.7.zip
    DOWNLOAD_NAME jsoncpp-0.10.7.zip
    SOURCE_DIR jsoncpp
    CONFIGURE_COMMAND cd ${PROJECT_BINARY_DIR}/jsoncpp && cmake . ${CROSS_COMPLIE_OPTION} -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/jsoncpp/build
    BUILD_IN_SOURCE 1
    BUILD_COMMAND make ${FAST_COMPLIE_OPTION}
    INSTALL_COMMAND make install && cp ${PROJECT_BINARY_DIR}/jsoncpp/build/include ${PROJECT_BINARY_DIR} -rf && cp ${PROJECT_BINARY_DIR}/jsoncpp/build/lib ${PROJECT_BINARY_DIR} -rf
    )
#easyloggrt
ExternalProject_Add(EasyLogger
    URL https://github.com/lam2003/monitor_3rdparty/raw/master/EasyLogger.tar.gz
    DOWNLOAD_NAME EasyLogger.tar.gz
    SOURCE_DIR EasyLogger
    CONFIGURE_COMMAND cd ${PROJECT_BINARY_DIR}/EasyLogger && cmake . ${CROSS_COMPLIE_OPTION} -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/EasyLogger/build
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