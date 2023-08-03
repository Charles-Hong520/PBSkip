#ifndef __LIB_H__
#define __LIB_H__
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <iostream>
#include <string>
inline void handle_error(const char* msg) {
  perror(msg);
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
    this->end = start + length;
    this->curr = start;
    this->buffer = buffer.buffer;  // always 0th index/beg of file
  }
  Seeker(Seeker& s, size_t length) {
    this->buffer = s.buffer;
    this->curr = s.curr;
    this->end = s.curr + length;
  }
  bool ReadVarint32(uint32_t* i) {
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
  inline void ReadVarint64(uint64_t* i) {
    *i = 0;
    uint32_t shift = 0;
    while (curr < end) {
      uint64_t b = buffer[curr];
      curr++;
      *i |= (b & 0x7f) << shift;
      if (b < 0x80) {
        return;
      }
      shift += 7;
    }
    handle_error("ReadVarint64: eof");
  }

  inline void ReadString(std::string* str, uint32_t len) {
    if (curr + len > end) {
      handle_error("ReadString: eof");
    }
    str->assign((char*)buffer + curr, len);
    curr += len;
  }
};

#endif