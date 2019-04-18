add_library(video_process 
    osd.cpp
    video_process_impl.cpp
)


add_dependencies(video_process
    common
    SDL2_ttf
)


