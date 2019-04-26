add_library(live 
    rtmp.cpp
    rtmp_streamer.cpp
    )

add_dependencies(live
    common
    libav
    )