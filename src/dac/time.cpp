#include <fcntl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/message_differencer.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "../dac/profile.pbs.h"
#include "../schema/profile.pb.h"
/*
g++ -std=c++17 src/dac/profile.pbs.cpp src/schema/profile.pb.cc src/dac/time.cpp -lprotobuf -lpthread -O3 -o src/dac/testprofile && src/dac/testprofile
*/
int main() {
  int PBS_NUM_THREADS = 2;
  if (const auto env_p = std::getenv("PBS_NUM_THREADS")) {
    PBS_NUM_THREADS = std::stoi(env_p);
  }
  std::cout << "threads: " << PBS_NUM_THREADS << " \n";
  // std::cout << "---------------------Running the parallel version----------------------\n";
  std::string file = "dataset/zz.prof";
  Buffer content(file);

  std::chrono::time_point<std::chrono::system_clock> start_T, end_T;
  std::chrono::duration<double> elapsed_seconds;
  //-----------------------------------

  // parses our PBSkip object

  // std::cout << "Parsing " << file << " with custom pthreads" << std::endl;
  // std::cout << "content size: " << content.size << std::endl;

  Seeker seeker(content, 0, content.size);

  PBS::Profile* pbs = new PBS::Profile();
  pbs->fillTracker(seeker);
  auto tr = pbs->getTracker();
  std::map<int, int> pos_to_fi;

  int z = 0;
  for (const auto& [field_id, vpos] : tr) {
    std::cout << "field_id: " << field_id << ", size: " << vpos.size() << std::endl;
    for (auto& [pos, len] : vpos) {
      pos_to_fi[pos] = len;
      if (pos >= 1078689392 - 100 && pos <= 1078689392 + 400) {
        print("field_id", field_id, "pos:", pos, "len:", len);
      }
      z++;
    }
  }
  // 1078689392 299
  const int startCheck = 1078689392 - 10;
  const int endCheck = 1078689392 + 300;
  start_T = std::chrono::system_clock::now();
  for (int i = startCheck; i < endCheck; i++) {
    seeker.curr = i;
    seeker.end = content.size;
    if (pbs->checkValidStart(seeker)) {
      print("valid at", i);
    }
  }
  end_T = std::chrono::system_clock::now();

  seeker.curr = 0;
  // pbs->parseProfile(seeker);
  elapsed_seconds = end_T - start_T;
  std::cout << "time: " << elapsed_seconds.count() << "s\n";
}