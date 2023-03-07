set(
    src
    net/IpAddress.cpp
    net/SocketCommon.cpp
    net/Socket.cpp
)

if (UNIX)
  link_libraries(pthread)
else ()
  link_libraries(ws2_32 wsock32)
endif ()
add_library(potato ${src})
include_directories(${PROJECT_SOURCE_DIR})