include_directories(${PROJECT_SOURCE_DIR})

add_executable(IpAddressTest IpAddressTest.cpp)
target_link_libraries(IpAddressTest PRIVATE potato)

add_executable(BufferTest BufferTest.cpp)
target_link_libraries(BufferTest PRIVATE potato)

add_executable(EchoServerTest EchoServerTest.cpp)
target_link_libraries(EchoServerTest PRIVATE potato)
