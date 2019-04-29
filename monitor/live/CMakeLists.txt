add_library(live 
    rtmp.cpp
    rtmp_streamer.cpp
    rtmp_streamer2.cpp
    )

add_dependencies(live
    common
    )