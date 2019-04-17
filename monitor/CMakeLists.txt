add_subdirectory(video_capture)
add_subdirectory(video_process)
add_subdirectory(common)

add_executable(monitor main.cpp)

set(MPP_LIBS 
    mpi
    ive 
    md 
    VoiceEngine 
    upvqe 
    dnvqe 
    _hiae 
    isp 
    sns_imx290 
    _hiae 
    _hiawb 
    _hiaf 
    _hidefog
    )

set(SELF_LIBS 
    video_capture
    video_process
    common
    )
set(3RDPARTY 
    easylogger
    pthread
    dl
    m
    )

target_link_libraries(monitor 
    ${MPP_LIBS} 
    ${SELF_LIBS} 
    ${3RDPARTY}
    )
