#ifndef __PERSON_PBS_H__
#define __PERSON_PBS_H__

#include <string>
#include <vector>

#include "lib.hpp"

class Address {
 private:
  uint32_t zipcode;
  bool international;
  std::string city;

 public:
  void set_zipcode(uint32_t);
  void set_international(bool);
  void set_city(const std::string&);
  bool parseAddress(Seeker&);
};

class Person {
 private:
  std::string name;
  int64_t age;
  Address* address;
  std::vector<std::string> names;
  std::vector<uint64_t> ages;
  std::vector<Address*> addresses;

 public:
  void set_name(const std::string&);
  void set_age(int64_t);
  void set_address(Address*);
  void add_names(const std::string&);
  void add_ages(int64_t);
  void add_addresses(Address*);
  bool parsePerson(Seeker&);
};

#endif