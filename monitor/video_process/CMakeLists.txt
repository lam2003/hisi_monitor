add_library(video_process osd.cpp)
# add_dependencies(video_process SDL2_ttf SDL2 freetype)

target_link_libraries(video_process 
    libSDL2_ttf.a 
    libSDL2.a 
    libfreetype.a
    )