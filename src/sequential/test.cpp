#include "person.pbs.h"

int main() {
  std::string filename = "dataset/person1.bin";
  Buffer content(filename);
  Seeker seeker(content, 0, content.size);
  Person p;
  std::cout << "buffer size: " << content.size << std::endl;
  for (int i = 0; i < content.size; i++) {
    std::cout << (int)content.buffer[i] << " ";
  }
  p.parsePerson(seeker);
  std::cout << p << std::endl;
}