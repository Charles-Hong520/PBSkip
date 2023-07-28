#include "person.pbs.h"
void Address::set_zipcode(uint32_t zipcode) {
  this->zipcode = zipcode;
}
void Address::set_international(bool b) {
  this->international = b;
}
void Address::set_city(const std::string& str) {
  this->city = str;
}
bool Address::parseAddress(Seeker& seek) {
  uint32_t tag;

  return true;
}

void Person::set_name(const std::string& str) {
  this->name = str;
}
void Person::set_age(int64_t age) {
  this->age = age;
}
void Person::set_address(Address* addr) {
  this->address = addr;
}
void Person::add_names(const std::string& str) {
  this->names.push_back(str);
}
void Person::add_ages(int64_t age) {
  this->ages.push_back(age);
}
void Person::add_addresses(Address* addr) {
  this->addresses.push_back(addr);
}
bool Person::parsePerson(Seeker&) {
  return true;
}
// bool Person::parsePerson(Seeker& seek) {
//   uint32_t tag = input->ReadTag();
//   // cout<<input->CurrentPosition()<<endl;
//   if (tag == 0) {
//     // encounter EOF or tag == 0
//     // 1. EOF: when ReadTag() returns 0 when called exactly at EOF
//     // 2. tag parsed as 0: a byte 0x00 is interpreted as a tag, indicating a wrong parse
//     return false;
//   }
//   uint32_t field_id = tag >> 3;
//   uint32_t wire = tag & 7;
//   // handle a record
//   switch (field_id) {
//     case 1:  // name
//       if (wire == 2) {
//         uint32_t len;
//         input->ReadVarint32(&len);
//         std::string buffer;
//         input->ReadString(&buffer, len);
//         person.set_name(buffer);
//       }
//       break;
//     case 2:  // age
//       if (wire == 0) {
//         uint32_t i;
//         input->ReadVarint32(&i);
//         person.set_age(i);
//       }
//       break;
//     case 3:  // phone number
//       if (wire == 2) {
//         // repeat email
//         uint32_t len;
//         input->ReadVarint32(&len);
//         std::string buffer;
//         input->ReadString(&buffer, len);
//         person.set_phone_numbers(buffer);
//       }
//       break;
//     case 4:  // hobby
//       if (wire == 2) {
//         uint32_t len;
//         input->ReadVarint32(&len);
//         std::string buffer;
//         input->ReadString(&buffer, len);
//         person.set_hobby(buffer);
//       }
//       break;
//     case 5:  // message addr
//       if (wire == 2) {
//         uint32_t len;
//         input->ReadVarint32(&len);

//         uint32_t currPos = input->CurrentPosition();
//         Address* addr = person.add_addr();
//         input->Skip(len);

//         // thread pool

//         // trds[i]=std::thread(&Parser::give_to_worker, this, currPos, len, addr);
//         // pool.push_task(&Parser::give_to_worker, this, currPos, len, addr);

//         // seq
//         gp::io::CodedInputStream* copyStream = new gp::io::CodedInputStream(
//             content + currPos, len);
//         addr->ParseFromCodedStream(copyStream);
//       }
//       break;
//   }
//   return true;
// }