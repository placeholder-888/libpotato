#ifndef POTATO_POTATO_UTILS_NONCOPYABLE_H_
#define POTATO_POTATO_UTILS_NONCOPYABLE_H_
namespace potato {

class NonCopyable {
protected:
  NonCopyable() = default;
  ~NonCopyable() = default;

public:
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;
};

} // end namespace potato
#endif // POTATO_POTATO_UTILS_NONCOPYABLE_H_
