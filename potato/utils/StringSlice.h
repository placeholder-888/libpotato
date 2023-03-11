#ifndef POTATO_POTATO_UTILS_STRINGSLICE_H_
#define POTATO_POTATO_UTILS_STRINGSLICE_H_

#include <cassert>
#include <cstring>
#include <string>
#include <vector>

namespace potato {

class StringSlice {
public:
  StringSlice(const char *str) : data_(str), len_(strlen(str)) {}
  StringSlice(const std::string &str) : data_(str.data()), len_(str.size()) {}
  StringSlice(const char *str, size_t len) : data_(str), len_(len) {}

  const char *data() const { return data_; }

  const char *begin() const { return data_; }
  const char *end() const { return data_ + len_; }

  size_t size() const { return len_; }
  size_t length() const { return len_; }
  bool empty() const { return len_ == 0; }

  const char &operator[](size_t index) const {
    assert(index < len_);
    return data_[index];
  }

  StringSlice slice(size_t beginIndex, size_t len = SIZE_MAX) const {
    size_t res = std::min(len_ - beginIndex, len);
    return {data_ + beginIndex, res};
  }

  std::string toString() const {
    if (len_ == 0)
      return "";
    return {data_, len_};
  }

  StringSlice trim(char delim = ' ') const {
    size_t front, back;
    for (front = 0; front < len_; ++front) {
      if (data_[front] != delim)
        break;
    }
    if (len_ == 0 || front == len_)
      return {"", 0};
    for (back = len_ - 1; back > front; --back) {
      if (data_[back] != delim)
        break;
    }
    return slice(front, back - front + 1);
  }

  std::vector<StringSlice> split(char delim) const {
    std::vector<StringSlice> ret;
    if (empty())
      return ret;
    size_t begin = 0;
    auto size = length();
    for (size_t i = 0; i < size; ++i) {
      if (data_[i] == delim) {
        ret.emplace_back(slice(begin, i - begin));
        begin = i + 1;
      }
    }
    ret.emplace_back(slice(begin, size - begin));
    return ret;
  }

  const char *find(const StringSlice &slice) {
    if (slice.empty() || empty())
      return nullptr;
    std::vector<size_t> next(slice.size(), 0);
    size_t j = 0;
    for (size_t i = 1; i < slice.size(); ++i) {
      while (j > 0 && slice[i] != slice[j])
        j = next[j - 1];
      if (slice[i] == slice[j])
        ++j;
      next[i] = j;
    }
    j = 0;
    for (size_t i = 0; i < len_; ++i) {
      while (j > 0 && slice[j] != data_[i])
        j = next[j - 1];
      if (slice[j] == data_[i]) {
        if (++j >= slice.size()) {
          return data_ + i - j + 1;
        }
      }
    }
    return nullptr;
  }

  const char *find(char c) {
    for (size_t i = 0; i < len_; ++i) {
      if (data_[i] == c)
        return data_ + i;
    }
    return nullptr;
  }

private:
  const char *data_;
  size_t len_;
};

inline bool operator==(const StringSlice &slice1, const StringSlice &slice2) {
  return slice1.length() == slice2.length() &&
         (memcmp(slice1.begin(), slice2.begin(), slice2.length()) == 0);
}

inline bool operator!=(const StringSlice &slice1, const StringSlice &slice2) {
  return !(slice1 == slice2);
}

inline std::ostream &operator<<(std::ostream &out, const StringSlice &slice) {
  out << slice.toString();
  return out;
}

} // end namespace potato
#endif // POTATO_POTATO_UTILS_STRINGSLICE_H_
