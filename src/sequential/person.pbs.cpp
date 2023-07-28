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
                    uint64_t i;
                    seek.ReadVarint64(&i);
                    address.set_zipcode(i);
                }
                break;
            case 2:  // international
                if (wire == 0) {
                    uint64_t i;
                    seek.ReadVarint64(&i);
                    address.set_international(i);
                }
                break;
            case 3:  // city
                if (wire == 2) {
                    uint32_t len;
                    seek.ReadVarint32(&len);
                    std::string buffer;
                    seek.ReadString(&buffer, len);
                    address.set_city(buffer);
                }
                break;
        }
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
                    person.set_name(buffer);
                }
                break;
            case 2:  // age
                if (wire == 0) {
                    uint64_t i;
                    seek.ReadVarint64(&i);
                    person.set_age(i);
                }
                break;
            case 3:  // address
                if (wire == 2) {
                    uint32_t len;
                    seek.ReadVarint32(&len);
                    Address* addr;
                    addr->parseAddress(seek);
                    person.set_address(addr);
                }
                break;
            case 4:  // repeated names
                if (wire == 2) {
                    uint32_t len;
                    seek.ReadVarint32(&len);
                    std::string buffer;
                    seek.ReadString(&buffer, len);
                    person.add_names(buffer);
                }
            case 5:  // repeated ages
                if (wire == 0) {
                    uint64_t i;
                    seek.ReadVarint64(&i);
                    person.add_ages(i);
                }
                break;
            case 6:  // repeated addresses
                if (wire == 2) {
                    uint32_t len;
                    seek.ReadVarint32(&len);
                    Address* addr;
                    addr->parseAddress(seek);
                    person.add_addresses(addr);
                }
                break;
        }
    }
    return true;
}