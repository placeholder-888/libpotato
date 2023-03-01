set(
    src
    net/IpAddress.h
    net/SocketCommon.h
    utils/StringSlice.h
    utils/Endian.h
)

add_executable(main main.cpp ${src})