#ifndef __LIB_HPP__
#define __LIB_HPP__
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <iostream>

struct Buffer {
  uint8_t* buffer;
  size_t size;

  Buffer(std::string& filename) {
    int fd = open(filename.c_str(), O_RDONLY);
    struct stat st;
    stat(filename.c_str(), &st);
    this->size = st.st_size;
    this->buffer = (uint8_t*)mmap(0, size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
  }
};

struct Seeker {
  uint8_t* buffer;
  uint32_t curr;
  uint32_t end;

  Seeker(const Buffer& buffer, size_t start, size_t length) {
    if (buffer.size - length < start) {
      std::cout << "ERROR: Seeker Constructor: end is past eof" << std::endl;
      exit(0);
    }
    this->end = start + length;
    this->curr = start;
    this->buffer = buffer.buffer;  // always 0th index/beg of file
  }

  bool ReadVarint32(uint32_t* i) {
    *i = 0;
    uint32_t shift = 0;
    while (curr < end) {
      uint32_t b = buffer[curr];
      curr++;
      *i |= (b & 0x7f) << shift;
      if ((b & 0x80) == 0) {
        return true;
      }
      shift += 7;
    }
    std::cout << "ERROR: ReadVarint32: end is past eof" << std::endl;
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
      if ((b & 0x80) == 0) {
        return;
      }
      shift += 7;
    }
    std::cout << "ERROR: ReadVarint64: end is past eof" << std::endl;
    exit(0);
  }

  inline void ReadString(std::string* str, uint32_t len) {
    if (curr + len > end) {
      std::cout << "ERROR: ReadString: end is past eof" << std::endl;
      exit(0);
    }
    str->assign((char*)buffer + curr, len);
    curr += len;
  }
};

#endif