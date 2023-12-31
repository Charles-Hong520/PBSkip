#ifndef __PERSON_PBS_H__
#define __PERSON_PBS_H__
#include <iostream>
#include <string>
#include <vector>

#include "../lib.h"

class Address {
 private:
  uint32_t zipcode;
  bool international;
  std::string city;

 public:
  Address() {
    zipcode = 0;
    international = false;
    city = "";
  }
  void set_zipcode(uint32_t);
  void set_international(bool);
  void set_city(const std::string&);
  bool parseAddress(Seeker&);
  friend std::ostream& operator<<(std::ostream& os, const Address& a);
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
  Person() {
    name = "";
    age = 0;
    address = nullptr;
  }
  void set_name(const std::string&);
  void set_age(int64_t);
  void set_address(Address*);
  void add_names(const std::string&);
  void add_ages(int64_t);
  void add_addresses(Address*);
  bool parsePerson(Seeker&);
  friend std::ostream& operator<<(std::ostream& os, const Person& p);
};

#endif