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
  uint32_t tag = seek.ReadTag();
  while (tag != 0) {
    uint32_t field_id = tag >> 3;
    uint32_t wire = tag & 7;

    switch (field_id) {
      case 1:  // zipcode
        if (wire == 0) {
          uint32_t i;
          seek.ReadVarint32(&i);
          set_zipcode(i);
        }
        break;
      case 2:  // international
        if (wire == 0) {
          uint32_t i;
          seek.ReadVarint32(&i);
          set_international(i);
        }
        break;
      case 3:  // city
        if (wire == 2) {
          uint32_t len;
          seek.ReadVarint32(&len);
          std::string buffer;
          seek.ReadString(&buffer, len);
          set_city(buffer);
        }
        break;
    }
    tag = seek.ReadTag();
  }
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
bool Person::parsePerson(Seeker& seek) {
  uint32_t tag = seek.ReadTag();
  while (tag != 0) {
    uint32_t field_id = tag >> 3;
    uint32_t wire = tag & 7;

    switch (field_id) {
      case 1:  // name
        if (wire == 2) {
          uint32_t len;
          seek.ReadVarint32(&len);
          std::string buffer;
          seek.ReadString(&buffer, len);
          set_name(buffer);
        }
        break;
      case 2:  // age
        if (wire == 0) {
          uint64_t i;
          seek.ReadVarint64(&i);
          set_age(i);
        }
        break;
      case 3:  // address
        if (wire == 2) {
          uint32_t len;
          seek.ReadVarint32(&len);
          Address* addr = new Address();
          Seeker copyseeker(seek, len);
          seek.curr += len;
          addr->parseAddress(copyseeker);
          set_address(addr);
        }
        break;
      case 4:  // repeated names
        if (wire == 2) {
          uint32_t len;
          seek.ReadVarint32(&len);
          std::string buffer;
          seek.ReadString(&buffer, len);
          add_names(buffer);
        }
        break;
      case 5:  // repeated ages
        if (wire == 2) {
          uint32_t len;
          seek.ReadVarint32(&len);
          while (len--) {
            uint64_t i;
            seek.ReadVarint64(&i);
            add_ages(i);
          }
        }
        break;
      case 6:  // repeated addresses
        if (wire == 2) {
          uint32_t len;
          seek.ReadVarint32(&len);
          Address* addr = new Address();
          Seeker copyseeker(seek, len);
          seek.curr += len;
          addr->parseAddress(copyseeker);
          add_addresses(addr);
        }
        break;
    }
    tag = seek.ReadTag();
    std::cout << field_id << std::endl;
  }
  return true;
}

std::ostream& operator<<(std::ostream& os, const Person& p) {
  os << "Person\n";
  os << "name: " << p.name << "\n";
  os << "age: " << p.age << "\n";
  if (p.address) os << "address: " << *p.address << "\n";
  os << "names: {\n";
  for (auto n : p.names) {
    os << "\t" << n << "\n";
  }
  os << "}\n";
  os << "ages: {\n";
  for (auto n : p.ages) {
    os << "\t" << n << "\n";
  }
  os << "}\n";
  os << "addresses: {\n";
  for (auto n : p.addresses) {
    os << "\t" << *n << "\n";
  }
  os << "}\n";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Address& a) {
  os << "Address\n";
  os << "zipcode: " << a.zipcode << "\n";
  os << "international: " << a.international << "\n";
  os << "city: " << a.city << "\n";
  return os;
}