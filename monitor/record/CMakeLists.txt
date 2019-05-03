add_library(record 
    mp4_muxer.cpp
    mp4_record.cpp
)

add_dependencies(record
    common
)