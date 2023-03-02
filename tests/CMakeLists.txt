include_directories(${PROJECT_SOURCE_DIR})

add_executable(SocketCommonTest SocketCommonTest.cpp)
target_link_libraries(SocketCommonTest PRIVATE potato)
