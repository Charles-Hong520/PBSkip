#ifndef __LIB_H__
#define __LIB_H__
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <initializer_list>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

inline void print() { std::cout << '\n'; }

template <typename T, typename... Args>
inline void print(const T& t, Args... args) {
  std::cout << t << ' ';
  print(args...);
}
inline void handle_error(std::string msg) {
  print(msg);
  exit(0);
}
struct Buffer {
  uint8_t* buffer;
  size_t size;

  Buffer(std::string& filename) {
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1)
      handle_error("open");

    // obtain file size
    struct stat sb;
    if (fstat(fd, &sb) == -1)
      handle_error("fstat");

    this->size = sb.st_size;

    this->buffer = static_cast<uint8_t*>(mmap(NULL, this->size, PROT_READ, MAP_PRIVATE, fd, 0u));
    if (this->buffer == MAP_FAILED)
      handle_error("mmap");

    // TODO close fd at some point in time, call munmap(...)
  }
};

struct Seeker {
  uint8_t* buffer;
  uint32_t curr;
  uint32_t end;

  Seeker(const Buffer& buffer, size_t start, size_t length) {
    if (buffer.size - length < start) {
      handle_error("Seeker Constructor");
    }
    this->curr = start;
    this->end = start + length;
    this->buffer = buffer.buffer;  // always 0th index/beg of file
  }
  Seeker(Seeker& s, size_t length) {
    this->buffer = s.buffer;
    this->curr = s.curr;
    this->end = s.curr + length;
  }
  Seeker(uint8_t* b, uint32_t start, size_t length) {
    this->buffer = b;
    this->curr = start;
    this->end = start + length;
  }
  inline void Skip(uint32_t len) {
    this->curr += len;
  }
  inline bool ReadVarint32(uint32_t* i) {
    *i = 0;
    uint32_t shift = 0;
    while (curr < end) {
      uint32_t b = buffer[curr];
      curr++;
      *i |= (b & 0x7f) << shift;
      if (b < 0x80) {
        return true;
      }
      shift += 7;
    }
    return false;
  }
  inline uint32_t ReadTag() {
    uint32_t tag = 0;
    if (!ReadVarint32(&tag)) return 0;
    return tag;
  }
  inline bool ReadVarint64(uint64_t* i) {
    *i = 0;
    uint32_t shift = 0;
    while (curr < end) {
      uint64_t b = buffer[curr];
      curr++;
      *i |= (b & 0x7f) << shift;
      if (b < 0x80) {
        return true;
      }
      shift += 7;
    }
    return false;
  }

  inline bool ReadString(std::string* str, uint32_t len) {
    if (curr + len > end) {
      return false;
    }
    str->assign((char*)buffer + curr, len);
    curr += len;
    return true;
  }
};

#endif