#include "person.pbs.h"
Address::Address() {
	zipcode = 0;
	international = false;
	city = "";
}
void Address::set_zipcode(uint32_t zipcode) {
	this->zipcode = zipcode;
}
uint32_t Address::get_zipcode() {
	return this->zipcode;
}
void Address::set_international(bool international) {
	this->international = international;
}
bool Address::get_international() {
	return this->international;
}
void Address::set_city(const std::string& city) {
	this->city = city;
}
std::string Address::get_city() {
	return this->city;
}
bool Address::parseAddress(Seeker& seek) {
	uint32_t tag = seek.ReadTag();
	while (tag != 0) {
		uint32_t field_id = tag >> 3;
		uint32_t wire = tag & 7;

		switch (field_id) {
		case 1:
			if (wire == 0) {
				uint32_t i;
				seek.ReadVarint32(&i);
				set_zipcode(i);
			}
			break;
		case 2:
			if (wire == 0) {
				uint32_t i;
				seek.ReadVarint32(&i);
				set_international(i);
			}
			break;
		case 3:
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
Person::Person() {
	name = "";
	age = 0;
	address = nullptr;
}
void Person::set_name(const std::string& name) {
	this->name = name;
}
std::string Person::get_name() {
	return this->name;
}
void Person::set_age(int64_t age) {
	this->age = age;
}
int64_t Person::get_age() {
	return this->age;
}
void Person::set_address(Address* address) {
	this->address = address;
}
Address* Person::get_address() {
	return this->address;
}
void Person::add_names(const std::string& names) {
	this->names.push_back(names);
}
std::vector<std::string> Person::get_names() {
	return this->names;
}
void Person::add_ages(uint64_t ages) {
	this->ages.push_back(ages);
}
std::vector<uint64_t> Person::get_ages() {
	return this->ages;
}
void Person::add_addresses(Address* addresses) {
	this->addresses.push_back(addresses);
}
std::vector<Address*> Person::get_addresses() {
	return this->addresses;
}
bool Person::parsePerson(Seeker& seek) {
	uint32_t tag = seek.ReadTag();
	while (tag != 0) {
		uint32_t field_id = tag >> 3;
		uint32_t wire = tag & 7;

		switch (field_id) {
		case 1:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			std::string buffer;
			seek.ReadString(&buffer, len);
			set_name(buffer);
		}
		break;
		case 2:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_age(i);
			}
			break;
		case 3:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			Address* msg_ = new Address();
			Seeker copyseeker(seek, len);
			seek.curr += len;
			msg_->parseAddress(copyseeker);
			set_address(msg_);
		}
		break;
		case 4:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			std::string buffer;
			seek.ReadString(&buffer, len);
			add_names(buffer);
		}
		break;
		case 5:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			uint64_t i;
			while (len--) {
				seek.ReadVarint64(&i);
				add_ages(i);
			}
		}
		break;
		case 6:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			Address* msg_ = new Address();
			Seeker copyseeker(seek, len);
			seek.curr += len;
			msg_->parseAddress(copyseeker);
			add_addresses(msg_);
		}
		break;
		}
    tag = seek.ReadTag();
	}
	return true;
}
