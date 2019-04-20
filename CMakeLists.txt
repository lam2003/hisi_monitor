cmake_minimum_required(VERSION 3.5)

#配置交叉编译
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER  arm-hisiv500-linux-gcc)
SET(CMAKE_CXX_COMPILER arm-hisiv500-linux-g++)

project(hisi_monitor
        VERSION 1.0.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_SKIP_RPATH true)
set(CMAKE_BUILD_TYPE "Debug")

#如果在选项中加入 -DALL_STATIC=1 使用静态gcc库以及静态g++库进行链接

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
message(STATUS "project will link against gcc-static g++-static")


#-fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE true)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=arm926ej-s -w -mno-unaligned-access -fno-aggressive-loop-optimizations -ffunction-sections -fdata-sections -Dhi3516ev100 -DSENSOR_TYPE=SONY_IMX290_MIPI_1080P_30FPS -DHI_RELEASE -DHI_XXXX -DISP_V2 -DHI_ACODEC_TYPE_INNER")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=arm926ej-s -w -mno-unaligned-access -fno-aggressive-loop-optimizations -ffunction-sections -fdata-sections -Dhi3516ev100 -DSENSOR_TYPE=SONY_IMX290_MIPI_1080P_30FPS -DHI_RELEASE -DHI_XXXX -DISP_V2 -DHI_ACODEC_TYPE_INNER")

# -D__CMAKE_FILE__='\"$(notdir $(abspath $<))\"'
include_directories(
    ${PROJECT_BINARY_DIR}/include
    ${PROJECT_SOURCE_DIR}/monitor
)

link_directories(
    ${PROJECT_BINARY_DIR}/lib
)

include(3rdparty.cmake)

add_compile_options(-include global.h)

add_subdirectory(monitor)

