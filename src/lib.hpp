#define __LIB_HPP__
#ifndef __LIB_HPP__

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

struct Buffer {
  uint8_t* buffer;
  uint32_t size;

  Buffer(std::string& filename, uint32_t size) {
    this->buffer = buffer;
    this->size = size;

    constexpr size_t MAX_CONTENT_SIZE = 1e9;  // Set an appropriate maximum content size
    content = new uint8_t[MAX_CONTENT_SIZE];
    content_size = 0;

    std::ifstream file(filename, std::ios::binary);
    struct stat st;
    stat(filename.c_str(), &st);
    content_size = st.st_size;
    int fhand = open(filename.c_str(), O_RDONLY);
    content = (uint8_t*)mmap(0, content_size, PROT_READ, MAP_FILE | MAP_PRIVATE, fhand, 0);
  }
};

struct Seeker {
  uint8_t* buffer;
  uint32_t curr;
  uint32_t end;

  Seeker(const Buffer& buffer, uint32_t start, uint32_t length) {
    if (buffer.size - length < start) {
      cout << "ERROR: Seeker Constructor: end is past eof" << endl;
      exit(0);
    }
    this->end = start + length;
    this->curr = start;
    uint8_t* buffer = buffer->buffer;  // always 0th index/beg of file
  }
};

#endif