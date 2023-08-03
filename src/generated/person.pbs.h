#ifndef __PERSON_H__
#define __PERSON_H__
#include <string>
#include <vector>
#include <iostream>
#include "../lib.h"
namespace PBS {
class Address {
private:
	uint32_t zipcode;
	bool international;
	std::string city;
public:
	Address();
	void set_zipcode(uint32_t);
	uint32_t get_zipcode();
	void set_international(bool);
	bool get_international();
	void set_city(const std::string&);
	std::string get_city();
	bool parseAddress(Seeker&);
	friend std::ostream& operator<<(std::ostream& os, const Address&);
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
	Person();
	void set_name(const std::string&);
	std::string get_name();
	void set_age(int64_t);
	int64_t get_age();
	void set_address(Address*);
	Address* get_address();
	void add_names(const std::string&);
	std::vector<std::string> get_names();
	void add_ages(uint64_t);
	std::vector<uint64_t> get_ages();
	void add_addresses(Address*);
	std::vector<Address*> get_addresses();
	bool parsePerson(Seeker&);
	friend std::ostream& operator<<(std::ostream& os, const Person&);
};
}
#endif
