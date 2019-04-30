add_library(record 
    mp4_muxer.cpp
    mp4_record.cpp
    mp4_muxer2.cpp
)

add_dependencies(record
    common
)