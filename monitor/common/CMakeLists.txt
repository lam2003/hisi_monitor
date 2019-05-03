add_library(common 
    config.cpp
    system.cpp
)

add_dependencies(common
    EasyLogger
    hisi_sdk
    libav
)
