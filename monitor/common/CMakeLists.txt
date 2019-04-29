add_library(common 
    config.cpp
    system.cpp
    server_sock.cpp
)

add_dependencies(common
    EasyLogger
    hisi_sdk
    libav
)
