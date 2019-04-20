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
    #hisi
    libmpi.so
    libive.so 
    libmd.so 
    libVoiceEngine.so 
    libupvqe.so
    libdnvqe.so
    lib_hiae.so 
    libisp.so 
    libsns_imx290.so
    lib_hiae.so
    lib_hiawb.so 
    lib_hiaf.so 
    lib_hidefog.so
    #system
    pthread
    dl
    m
    #thirdparty
    libeasylogger.a
    libSDL2_ttf.a
    libSDL2.a 
    libfreetype.a
    #self
    common
    video_capture
    video_process
)




