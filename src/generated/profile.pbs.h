#ifndef __PROFILE_H__
#define __PROFILE_H__
#include <string>
#include <vector>
#include <iostream>
#include "../lib.h"
class Profile {
private:
	std::vector<ValueType*> sample_type;
	std::vector<Sample*> sample;
	std::vector<Mapping*> mapping;
	std::vector<Location*> location;
	std::vector<Function*> function;
	std::vector<std::string> string_table;
	int64_t drop_frames;
	int64_t keep_frames;
	int64_t time_nanos;
	int64_t duration_nanos;
	ValueType* period_type;
	int64_t period;
	std::vector<int64_t> comment;
	int64_t default_sample_type;
public:
	Profile();
	void add_sample_type(ValueType*);
	void add_sample(Sample*);
	void add_mapping(Mapping*);
	void add_location(Location*);
	void add_function(Function*);
	void add_string_table(const std::string&);
	void set_drop_frames(int64_t);
	void set_keep_frames(int64_t);
	void set_time_nanos(int64_t);
	void set_duration_nanos(int64_t);
	void set_period_type(ValueType*);
	void set_period(int64_t);
	void add_comment(int64_t);
	void set_default_sample_type(int64_t);
	bool parseProfile(Seeker&);
	friend std::ostream& operator<<(std::ostream& os, const Profile&);
};
class ValueType {
private:
	int64_t type;
	int64_t unit;
public:
	ValueType();
	void set_type(int64_t);
	void set_unit(int64_t);
	bool parseValueType(Seeker&);
	friend std::ostream& operator<<(std::ostream& os, const ValueType&);
};
class Sample {
private:
	std::vector<uint64_t> location_id;
	std::vector<int64_t> value;
	std::vector<Label*> label;
public:
	Sample();
	void add_location_id(uint64_t);
	void add_value(int64_t);
	void add_label(Label*);
	bool parseSample(Seeker&);
	friend std::ostream& operator<<(std::ostream& os, const Sample&);
};
class Label {
private:
	int64_t key;
	int64_t str;
	int64_t num;
	int64_t num_unit;
public:
	Label();
	void set_key(int64_t);
	void set_str(int64_t);
	void set_num(int64_t);
	void set_num_unit(int64_t);
	bool parseLabel(Seeker&);
	friend std::ostream& operator<<(std::ostream& os, const Label&);
};
class Mapping {
private:
	uint64_t id;
	uint64_t memory_start;
	uint64_t memory_limit;
	uint64_t file_offset;
	int64_t filename;
	int64_t build_id;
	bool has_functions;
	bool has_filenames;
	bool has_line_numbers;
	bool has_inline_frames;
public:
	Mapping();
	void set_id(uint64_t);
	void set_memory_start(uint64_t);
	void set_memory_limit(uint64_t);
	void set_file_offset(uint64_t);
	void set_filename(int64_t);
	void set_build_id(int64_t);
	void set_has_functions(bool);
	void set_has_filenames(bool);
	void set_has_line_numbers(bool);
	void set_has_inline_frames(bool);
	bool parseMapping(Seeker&);
	friend std::ostream& operator<<(std::ostream& os, const Mapping&);
};
class Location {
private:
	uint64_t id;
	uint64_t mapping_id;
	uint64_t address;
	std::vector<Line*> line;
	bool is_folded;
public:
	Location();
	void set_id(uint64_t);
	void set_mapping_id(uint64_t);
	void set_address(uint64_t);
	void add_line(Line*);
	void set_is_folded(bool);
	bool parseLocation(Seeker&);
	friend std::ostream& operator<<(std::ostream& os, const Location&);
};
class Line {
private:
	uint64_t function_id;
	int64_t line;
public:
	Line();
	void set_function_id(uint64_t);
	void set_line(int64_t);
	bool parseLine(Seeker&);
	friend std::ostream& operator<<(std::ostream& os, const Line&);
};
class Function {
private:
	uint64_t id;
	int64_t name;
	int64_t system_name;
	int64_t filename;
	int64_t start_line;
public:
	Function();
	void set_id(uint64_t);
	void set_name(int64_t);
	void set_system_name(int64_t);
	void set_filename(int64_t);
	void set_start_line(int64_t);
	bool parseFunction(Seeker&);
	friend std::ostream& operator<<(std::ostream& os, const Function&);
};
#endif
