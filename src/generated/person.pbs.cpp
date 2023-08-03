#include "person.pbs.h"
Address::Address() {
	zipcode = 0;
	international = false;
	city = "";
}
void Address::set_zipcode(uint32_t zipcode) {
	this->zipcode = zipcode;
}
void Address::set_international(bool international) {
	this->international = international;
}
void Address::set_city(const std::string& city) {
	this->city = city;
}
Person::Person() {
	name = "";
	age = 0;
	address = nullptr;
}
void Person::set_name(const std::string& name) {
	this->name = name;
}
void Person::set_age(int64_t age) {
	this->age = age;
}
void Person::set_address(Address* address) {
	this->address = address;
}
void Person::add_names(const std::string& names) {
	this->names.push_back(names);
}
void Person::add_ages(uint64_t ages) {
	this->ages.push_back(ages);
}
void Person::add_addresses(Address* addresses) {
	this->addresses.push_back(addresses);
}
