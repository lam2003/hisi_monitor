add_subdirectory(common)
add_subdirectory(video_capture)
add_subdirectory(video_process)


add_executable(monitor 
    main.cpp
)

add_dependencies(monitor
    common
    video_capture
    video_process
)


target_link_libraries(monitor
    common
    video_capture
    video_process
    libmpi.so
    libive.a 
    libmd.a 
    libVoiceEngine.a 
    libupvqe.a
    libdnvqe.a 
    lib_hiae.a 
    libisp.a 
    libsns_imx290.so
    lib_hiae.a
    lib_hiawb.a 
    lib_hiaf.a 
    lib_hidefog.a
    libeasylogger.a
    libSDL2_ttf.a
    libSDL2.a 
    libfreetype.a
    pthread
    dl
    m
)




