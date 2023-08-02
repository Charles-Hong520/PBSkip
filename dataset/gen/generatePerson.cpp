#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/message_differencer.h>

#include <fstream>
#include <iostream>

#include "../../src/schema/person.pb.h"
/*
g++ -std=c++17 dataset/gen/generatePerson.cpp src/schema/person.pb.cc -lprotobuf -lpthread -o dataset/gen/gen_person && ./dataset/gen/gen_person
g++ -std=c++17 dataset/gen/generatePerson.cpp src/schema/person.pb.cc -lprotobuf -lpthread -o dataset/gen/gen_person

*/

void fillAddress(Address* addr, int var);
int main() {
  {
    Person p;

    p.set_name("name");
    p.set_age(22);
    Address* addr = p.mutable_address();
    p.add_names("multname1");
    p.add_names("multname2");
    p.add_ages(100);
    p.add_ages(101);
    Address* addrs1 = p.add_addresses();
    Address* addrs2 = p.add_addresses();

    fillAddress(addrs2, 2);
    fillAddress(addr, 0);
    fillAddress(addrs1, 1);

    std::string filename = "dataset/person1.bin";
    std::fstream output(filename, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!p.SerializeToOstream(&output)) {
      std::cerr << "Failed to write " << filename << std::endl;
      return 0;
    }
    output.close();
  }
}

void fillAddress(Address* addr, int var) {
  std::string str_var = std::to_string(var);
  addr->set_zipcode(92507 + var);
  addr->set_international((bool)(var % 2));
  addr->set_city("Riverside, CA" + str_var);
}
