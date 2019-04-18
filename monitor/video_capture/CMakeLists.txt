add_library(video_capture 
    video_capture_impl.cpp
)

add_dependencies(video_capture
    common
)