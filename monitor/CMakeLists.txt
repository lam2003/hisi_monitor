add_subdirectory(video_capture)
add_subdirectory(common)

add_executable(monitor main.cpp)

set(MPP_LIBS mpi ive md VoiceEngine upvqe dnvqe _hiae isp sns_imx290 _hiae _hiawb _hiaf _hidefog)

target_link_libraries(monitor video_capture common easylogger ${MPP_LIBS} pthread dl m)

