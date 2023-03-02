#ifndef POTATO_POTATO_UTILS_ENDIAN_H_
#define POTATO_POTATO_UTILS_ENDIAN_H_
#include <cstdint>
#if defined(linux) || defined(__linux) || defined(__linux__)
#include <endian.h>
#define potato_htonll(x) htobe64(x)
#define potato_htonl(x) htobe32(x)
#define potato_htons(x) htobe16(x)
#define potato_ntohll(x) be64toh(x)
#define potato_ntohl(x) be32toh(x)
#define potato_ntohs(x) be16toh(x)
#elif defined(WIN64) || defined(_WIN64) || defined(__WIN64__) ||               \
    defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#include <winsock2.h>
#define HTONLL(x)                                                              \
  ((1 == htonl(1))                                                             \
       ? (x)                                                                   \
       : ((static_cast<uint64_t>(htonl((x)&0xFFFFFFFFUL))) << 32) |            \
             htonl(static_cast<uint32_t>((x) >> 32)))

#define NTOHLL(x)                                                              \
  ((1 == ntohl(1))                                                             \
       ? (x)                                                                   \
       : ((static_cast<uint64_t>(ntohl((x)&0xFFFFFFFFUL))) << 32) |            \
             ntohl(static_cast<uint32_t>((x) >> 32)))

#define potato_htonll(x) HTONLL(x)
//FIXME MINGW找不到htonll?
//#define potato_htonll(x) htonll(x)
#define potato_htonl(x) htonl(x)
#define potato_htons(x) htons(x)
//FIXME MINGW找不到ntohll?
//#define potato_ntohll(x) ntohll(x)
#define potato_ntohll(x) NTOHLL(x)
#define potato_ntohl(x) ntohl(x)
#define potato_ntohs(x) ntohs(x)
#endif
namespace potato {
namespace endian {

inline uint16_t hostToNetwork16(uint16_t host16) {
  return potato_htons(host16);
}
inline uint32_t hostToNetwork32(uint32_t host32) {
  return potato_htonl(host32);
}

inline uint64_t hostToNetwork64(uint64_t host64) {
  return potato_htonll(host64);
}

inline uint16_t networkToHost16(uint16_t net16) { return potato_ntohs(net16); }
inline uint32_t networkToHost32(uint32_t net32) { return potato_ntohl(net32); }
inline uint64_t networkToHost64(uint64_t net64) { return potato_ntohll(net64); }

} // end namespace endian
} // end namespace potato
#endif // POTATO_POTATO_UTILS_ENDIAN_H_
