add_subdirectory(video_capture)
add_subdirectory(common)

add_executable(monitor main.cpp)

set(MPP_LIBS mpi ive md VoiceEngine upvqe dnvqe isp _hiae _hiawb _hiaf _hidefog sns_imx290)

target_link_libraries(monitor video_capture common easylogger pthread dl m ${MPP_LIBS})

