#ifndef POTATO_POTATO_NET_BUFFER_H_
#define POTATO_POTATO_NET_BUFFER_H_

#include "potato/utils/StringSlice.h"
#include <vector>

namespace potato {

class Buffer {
public:
  Buffer() : buf_(4096) {}
  ~Buffer() = default;

  void writeInt32(int32_t value);
  void writeInt64(int64_t value);
  void writeUint64(uint64_t value);

  int32_t readInt32() const;
  int64_t readInt64() const;
  uint64_t readUint64() const;

  int32_t retrieveInt32();
  int64_t retrieveInt64();
  uint64_t retrieveUint64();

  size_t writeableBytes() const {
    assert(curWriteIndex_ <= buf_.size());
    return buf_.size() - curWriteIndex_;
  }

  const char *peek() const { return buf_.data() + curReadIndex_; }
  const char *beginWrite() const { return buf_.data() + curWriteIndex_; }
  char *beginWrite() { return buf_.data() + curWriteIndex_; }

  void reset() { curWriteIndex_ = curReadIndex_ = 0; }

  void hasWritten(size_t len) {
    assert(len <= writeableBytes());
    curWriteIndex_ += len;
  }

  void retrieve(char *buf, size_t len);
  void retrieve(void *buf, size_t len);

  std::string retrieveAllAsString() {
    auto slice = readAsSlice();
    reset();
    return slice.toString();
  }

  void retrieveAll() { reset(); }

  void retrieve(size_t len) {
    assert(len <= readableBytes());
    if (len == readableBytes())
      reset();
    else
      curReadIndex_ += len;
  }

  void retrieveUntil(const char *end) {
    assert(peek() <= end);
    assert(end <= beginWrite());
    retrieve(static_cast<size_t>(end - peek()));
  }

  void write(StringSlice slice);
  void write(const char *data, size_t len);
  void write(const void *data, size_t len);

  size_t readableBytes() const {
    assert(curReadIndex_ <= curWriteIndex_);
    return curWriteIndex_ - curReadIndex_;
  }

  StringSlice readAsSlice();
  void read(char *data, size_t len);

  void expandBuffer(size_t len);

private:
  std::vector<char> buf_;
  size_t curReadIndex_{0};
  size_t curWriteIndex_{0};
};

} // namespace potato

#endif // POTATO_POTATO_NET_BUFFER_H_
