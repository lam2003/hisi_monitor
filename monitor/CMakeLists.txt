add_subdirectory(common)
add_subdirectory(video_capture)
add_subdirectory(video_process)
add_subdirectory(video_detect)
add_subdirectory(video_codec)
add_subdirectory(live)
add_subdirectory(record)

add_executable(monitor 
    main.cpp
)

add_dependencies(monitor
    common
    video_capture
    video_process
    video_detect
    video_codec
    live
    record
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
    libavformat.a
    # libavdevice.a
    libavcodec.a
    libavutil.a
    libavresample.a
    libev.a
    ${PROJECT_BINARY_DIR}/lib/srs_librtmp.a
    # librtmp.a
    libmp4v2.a
    libz.a
    libssl.a
    libcrypto.a
    #self
    common
    video_capture
    video_process
    video_detect
    video_codec
    live
    record
)




