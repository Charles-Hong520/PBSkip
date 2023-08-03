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
#include <vector>

#include "../generated/person.pbs.h"
#include "../schema/person.pb.h"
// #include <google/protobuf/port_undef.inc>

/*
g++ -std=c++17 src/generated/person.pbs.cpp src/schema/person.pb.cc src/sequential/testperson.cpp -lprotobuf -lpthread -O3 -o src/sequential/testperson && ./src/sequential/testperson
*/
int main() {
  std::string file = "dataset/person1.bin";
  Buffer content(file);
  PBS::Person pbs;
  Person person_correct;
  Person person_custom;

  std::chrono::time_point<std::chrono::system_clock> start_T, end_T;
  std::chrono::duration<double> elapsed_seconds;
  //-----------------------------------

  // parses our PBSkip object

  std::cout << "Parsing " << file << " with custom API" << std::endl;
  Seeker seeker(content, 0, content.size);
  pbs.parsePerson(seeker);

  //-----------------------------------

  // parses google protobuf message object

  std::cout << "Parsing " << file << " with Google API" << std::endl;
  std::fstream cinput(file, std::ios::in | std::ios::binary);
  person_correct.ParseFromIstream(&cinput);

  // google::protobuf::CodedInputStream* input = new google::protobuf::CodedInputStream(content.buffer, content.size);
  // person_correct.ParseFromCodedStream(input);

  // std::string s_correct;
  // parser.profile_correct.SerializeToString(&s_correct);
  // cout << "size correct: " << s_correct.size() << endl;

  //-----------------------------------
  // parses our object into the google message object to verify correctness
  std::cout << "Converting custom into google message" << std::endl;

  person_custom.set_name(pbs.get_name());
  person_custom.set_age(pbs.get_age());
  Address* aa = new Address();
  aa->set_zipcode(pbs.get_address()->get_zipcode());
  aa->set_international(pbs.get_address()->get_international());
  aa->set_city(pbs.get_address()->get_city());
  person_custom.set_allocated_address(aa);
  for (auto e : pbs.get_names()) {
    person_custom.add_names(e);
  }
  for (auto e : pbs.get_ages()) {
    person_custom.add_ages(e);
  }
  for (auto e : pbs.get_addresses()) {
    Address* aa1 = person_custom.add_addresses();
    aa1->set_zipcode(e->get_zipcode());
    aa1->set_international(e->get_international());
    aa1->set_city(e->get_city());
  }
  //-----------------------------------
  if (!google::protobuf::util::MessageDifferencer::Equivalent(person_custom, person_correct)) {
    std::cout << "MISMATCH" << std::endl;
  } else {
    std::cout << "MATCH" << std::endl;
  }
}