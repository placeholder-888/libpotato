#include "potato/net/Buffer.h"
#include "potato/utils/endian.h"

using potato::Buffer;

void Buffer::write(const char *data, size_t len) {
  if (len > writeableBytes()) {
    expandBuffer(len);
  }
  std::copy(data, data + len, buf_.data() + curWriteIndex_);
  curWriteIndex_ += len;
}

void Buffer::write(StringSlice slice) { write(slice.data(), slice.length()); }

void Buffer::write(const void *data, size_t len) {
  write(static_cast<const char *>(data), len);
}

void Buffer::read(char *data, size_t len) {
  assert(len <= readableBytes());
  std::copy(buf_.data() + curReadIndex_, buf_.data() + curReadIndex_ + len,
            data);
}

potato::StringSlice Buffer::readAsSlice() {
  return {buf_.data() + curReadIndex_, readableBytes()};
}

void Buffer::expandBuffer(size_t len) {
  size_t availSize = writeableBytes() + curReadIndex_;
  if (availSize > len) {
    std::copy(buf_.data() + curReadIndex_, buf_.data() + curWriteIndex_,
              buf_.begin());
    curWriteIndex_ = readableBytes();
    curReadIndex_ = 0;
  } else {
    buf_.resize(buf_.size() + len);
  }
}

void Buffer::writeInt32(int32_t value) {
  auto big = static_cast<int32_t>(
      endian::hostToNetwork32(static_cast<uint32_t>(value)));
  write(&big, sizeof(big));
}

void Buffer::writeInt64(int64_t value) {
  auto big = static_cast<int64_t>(
      endian::hostToNetwork64(static_cast<uint64_t>(value)));
  write(&big, sizeof(big));
}

void Buffer::writeUint64(uint64_t value) {
  auto big = endian::hostToNetwork64(value);
  write(&big, sizeof(big));
}

int32_t Buffer::readInt32() const {
  assert(readableBytes() >= sizeof(int32_t));
  int32_t big = 0;
  memcpy(&big, peek(), sizeof(int32_t));
  return static_cast<int32_t>(
      endian::networkToHost32(static_cast<uint32_t>(big)));
}

int64_t Buffer::readInt64() const {
  assert(readableBytes() >= sizeof(int64_t));
  int64_t big = 0;
  memcpy(&big, peek(), sizeof(int64_t));
  return static_cast<int64_t>(
      endian::networkToHost64(static_cast<uint64_t>(big)));
}

uint64_t Buffer::readUint64() const {
  assert(readableBytes() >= sizeof(uint64_t));
  uint64_t big = 0;
  memcpy(&big, peek(), sizeof(uint64_t));
  return static_cast<uint64_t>(endian::networkToHost64(big));
}

void Buffer::retrieve(char *buf, size_t len) {
  read(buf, len);
  retrieve(len);
}

void Buffer::retrieve(void *buf, size_t len) {
  retrieve(static_cast<char *>(buf), len);
  retrieve(len);
}

int32_t Buffer::retrieveInt32() {
  auto ret = readInt32();
  retrieve(sizeof(int32_t));
  return ret;
}

int64_t Buffer::retrieveInt64() {
  auto ret = readInt64();
  retrieve(sizeof(int64_t));
  return ret;
}

uint64_t Buffer::retrieveUint64() {
  auto ret = readUint64();
  retrieve(sizeof(uint64_t));
  return ret;
}
