#ifndef BUF_IO_H
#define BUF_IO_H
#include <cstdint>
#include <cstring>
#include <string>

struct byte_stream {
  byte_stream(void *d) : buf{static_cast<uint8_t *>(d)} {}
  template <typename T, typename = std::enable_if_t<!std::is_pointer_v<T> && std::is_pod_v<T>>>
  void write(const T &d) {
    std::memcpy(buf, static_cast<const void *>(&d), sizeof d);
    buf += sizeof(T);
  }
  void write(std::string s) { write(s.c_str()); }
  void write(const char *s) {
    while (*s)
      *buf++ = *s++;
    *buf++ = 0;
  }
  uint8_t *buf;
};

struct read_stream {
  read_stream(void *d) : buf{static_cast<uint8_t *>(d)} {}
  template <typename T, typename = std::enable_if_t<!std::is_pointer_v<T> && std::is_pod_v<T>>>
  T read() {
    T r = *reinterpret_cast<T *>(buf);
    buf += sizeof(T);
    return r;
  }
  template <typename T, typename = std::enable_if_t<std::is_same_v<T, std::string>>>
  std::string read() {
    std::string s;
    char c;
    while ((c = read<char>()))
      s.push_back(c);
    return s;
  }
  uint8_t *buf;
};

#endif // BUF_IO_H
