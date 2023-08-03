#include "profile.pbs.h"
Profile::Profile() {
	drop_frames = 0;
	keep_frames = 0;
	time_nanos = 0;
	duration_nanos = 0;
	period_type = nullptr;
	period = 0;
	default_sample_type = 0;
}
void Profile::add_sample_type(ValueType* sample_type) {
	this->sample_type.push_back(sample_type);
}
void Profile::add_sample(Sample* sample) {
	this->sample.push_back(sample);
}
void Profile::add_mapping(Mapping* mapping) {
	this->mapping.push_back(mapping);
}
void Profile::add_location(Location* location) {
	this->location.push_back(location);
}
void Profile::add_function(Function* function) {
	this->function.push_back(function);
}
void Profile::add_string_table(const std::string& string_table) {
	this->string_table.push_back(string_table);
}
void Profile::set_drop_frames(int64_t drop_frames) {
	this->drop_frames = drop_frames;
}
void Profile::set_keep_frames(int64_t keep_frames) {
	this->keep_frames = keep_frames;
}
void Profile::set_time_nanos(int64_t time_nanos) {
	this->time_nanos = time_nanos;
}
void Profile::set_duration_nanos(int64_t duration_nanos) {
	this->duration_nanos = duration_nanos;
}
void Profile::set_period_type(ValueType* period_type) {
	this->period_type = period_type;
}
void Profile::set_period(int64_t period) {
	this->period = period;
}
void Profile::add_comment(int64_t comment) {
	this->comment.push_back(comment);
}
void Profile::set_default_sample_type(int64_t default_sample_type) {
	this->default_sample_type = default_sample_type;
}
bool Profile::parseProfile(Seeker& seek) {
	uint32_t tag = seek.ReadTag();
	while (tag != 0) {
		uint32_t field_id = tag >> 3;
		uint32_t wire = tag & 7;

		switch (field_id) {
		case 1:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			ValueType* msg_ = new ValueType();
			Seeker copyseeker(seek, len);
			seek.curr += len;
			msg_->parseValueType(copyseeker);
			add_sample_type(msg_);
		}
		break;
		case 2:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			Sample* msg_ = new Sample();
			Seeker copyseeker(seek, len);
			seek.curr += len;
			msg_->parseSample(copyseeker);
			add_sample(msg_);
		}
		break;
		case 3:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			Mapping* msg_ = new Mapping();
			Seeker copyseeker(seek, len);
			seek.curr += len;
			msg_->parseMapping(copyseeker);
			add_mapping(msg_);
		}
		break;
		case 4:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			Location* msg_ = new Location();
			Seeker copyseeker(seek, len);
			seek.curr += len;
			msg_->parseLocation(copyseeker);
			add_location(msg_);
		}
		break;
		case 5:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			Function* msg_ = new Function();
			Seeker copyseeker(seek, len);
			seek.curr += len;
			msg_->parseFunction(copyseeker);
			add_function(msg_);
		}
		break;
		case 6:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			std::string buffer;
			seek.ReadString(&buffer, len);
			add_string_table(buffer);
		}
		break;
		case 7:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_drop_frames(i);
			}
			break;
		case 8:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_keep_frames(i);
			}
			break;
		case 9:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_time_nanos(i);
			}
			break;
		case 10:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_duration_nanos(i);
			}
			break;
		case 11:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			ValueType* msg_ = new ValueType();
			Seeker copyseeker(seek, len);
			seek.curr += len;
			msg_->parseValueType(copyseeker);
			set_period_type(msg_);
		}
		break;
		case 12:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_period(i);
			}
			break;
		case 13:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			uint64_t i;
			while (len--) {
				seek.ReadVarint64(&i);
				add_comment(i);
			}
		}
		break;
		case 14:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_default_sample_type(i);
			}
			break;
		}
    tag = seek.ReadTag();
	}
	return true;
}
ValueType::ValueType() {
	type = 0;
	unit = 0;
}
void ValueType::set_type(int64_t type) {
	this->type = type;
}
void ValueType::set_unit(int64_t unit) {
	this->unit = unit;
}
bool ValueType::parseValueType(Seeker& seek) {
	uint32_t tag = seek.ReadTag();
	while (tag != 0) {
		uint32_t field_id = tag >> 3;
		uint32_t wire = tag & 7;

		switch (field_id) {
		case 1:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_type(i);
			}
			break;
		case 2:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_unit(i);
			}
			break;
		}
    tag = seek.ReadTag();
	}
	return true;
}
Sample::Sample() {
}
void Sample::add_location_id(uint64_t location_id) {
	this->location_id.push_back(location_id);
}
void Sample::add_value(int64_t value) {
	this->value.push_back(value);
}
void Sample::add_label(Label* label) {
	this->label.push_back(label);
}
bool Sample::parseSample(Seeker& seek) {
	uint32_t tag = seek.ReadTag();
	while (tag != 0) {
		uint32_t field_id = tag >> 3;
		uint32_t wire = tag & 7;

		switch (field_id) {
		case 1:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			uint64_t i;
			while (len--) {
				seek.ReadVarint64(&i);
				add_location_id(i);
			}
		}
		break;
		case 2:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			uint64_t i;
			while (len--) {
				seek.ReadVarint64(&i);
				add_value(i);
			}
		}
		break;
		case 3:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			Label* msg_ = new Label();
			Seeker copyseeker(seek, len);
			seek.curr += len;
			msg_->parseLabel(copyseeker);
			add_label(msg_);
		}
		break;
		}
    tag = seek.ReadTag();
	}
	return true;
}
Label::Label() {
	key = 0;
	str = 0;
	num = 0;
	num_unit = 0;
}
void Label::set_key(int64_t key) {
	this->key = key;
}
void Label::set_str(int64_t str) {
	this->str = str;
}
void Label::set_num(int64_t num) {
	this->num = num;
}
void Label::set_num_unit(int64_t num_unit) {
	this->num_unit = num_unit;
}
bool Label::parseLabel(Seeker& seek) {
	uint32_t tag = seek.ReadTag();
	while (tag != 0) {
		uint32_t field_id = tag >> 3;
		uint32_t wire = tag & 7;

		switch (field_id) {
		case 1:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_key(i);
			}
			break;
		case 2:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_str(i);
			}
			break;
		case 3:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_num(i);
			}
			break;
		case 4:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_num_unit(i);
			}
			break;
		}
    tag = seek.ReadTag();
	}
	return true;
}
Mapping::Mapping() {
	id = 0;
	memory_start = 0;
	memory_limit = 0;
	file_offset = 0;
	filename = 0;
	build_id = 0;
	has_functions = false;
	has_filenames = false;
	has_line_numbers = false;
	has_inline_frames = false;
}
void Mapping::set_id(uint64_t id) {
	this->id = id;
}
void Mapping::set_memory_start(uint64_t memory_start) {
	this->memory_start = memory_start;
}
void Mapping::set_memory_limit(uint64_t memory_limit) {
	this->memory_limit = memory_limit;
}
void Mapping::set_file_offset(uint64_t file_offset) {
	this->file_offset = file_offset;
}
void Mapping::set_filename(int64_t filename) {
	this->filename = filename;
}
void Mapping::set_build_id(int64_t build_id) {
	this->build_id = build_id;
}
void Mapping::set_has_functions(bool has_functions) {
	this->has_functions = has_functions;
}
void Mapping::set_has_filenames(bool has_filenames) {
	this->has_filenames = has_filenames;
}
void Mapping::set_has_line_numbers(bool has_line_numbers) {
	this->has_line_numbers = has_line_numbers;
}
void Mapping::set_has_inline_frames(bool has_inline_frames) {
	this->has_inline_frames = has_inline_frames;
}
bool Mapping::parseMapping(Seeker& seek) {
	uint32_t tag = seek.ReadTag();
	while (tag != 0) {
		uint32_t field_id = tag >> 3;
		uint32_t wire = tag & 7;

		switch (field_id) {
		case 1:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_id(i);
			}
			break;
		case 2:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_memory_start(i);
			}
			break;
		case 3:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_memory_limit(i);
			}
			break;
		case 4:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_file_offset(i);
			}
			break;
		case 5:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_filename(i);
			}
			break;
		case 6:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_build_id(i);
			}
			break;
		case 7:
			if (wire == 0) {
				uint32_t i;
				seek.ReadVarint32(&i);
				set_has_functions(i);
			}
			break;
		case 8:
			if (wire == 0) {
				uint32_t i;
				seek.ReadVarint32(&i);
				set_has_filenames(i);
			}
			break;
		case 9:
			if (wire == 0) {
				uint32_t i;
				seek.ReadVarint32(&i);
				set_has_line_numbers(i);
			}
			break;
		case 10:
			if (wire == 0) {
				uint32_t i;
				seek.ReadVarint32(&i);
				set_has_inline_frames(i);
			}
			break;
		}
    tag = seek.ReadTag();
	}
	return true;
}
Location::Location() {
	id = 0;
	mapping_id = 0;
	address = 0;
	is_folded = false;
}
void Location::set_id(uint64_t id) {
	this->id = id;
}
void Location::set_mapping_id(uint64_t mapping_id) {
	this->mapping_id = mapping_id;
}
void Location::set_address(uint64_t address) {
	this->address = address;
}
void Location::add_line(Line* line) {
	this->line.push_back(line);
}
void Location::set_is_folded(bool is_folded) {
	this->is_folded = is_folded;
}
bool Location::parseLocation(Seeker& seek) {
	uint32_t tag = seek.ReadTag();
	while (tag != 0) {
		uint32_t field_id = tag >> 3;
		uint32_t wire = tag & 7;

		switch (field_id) {
		case 1:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_id(i);
			}
			break;
		case 2:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_mapping_id(i);
			}
			break;
		case 3:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_address(i);
			}
			break;
		case 4:
		if (wire == 2) {
			uint32_t len;
			seek.ReadVarint32(&len);
			Line* msg_ = new Line();
			Seeker copyseeker(seek, len);
			seek.curr += len;
			msg_->parseLine(copyseeker);
			add_line(msg_);
		}
		break;
		case 5:
			if (wire == 0) {
				uint32_t i;
				seek.ReadVarint32(&i);
				set_is_folded(i);
			}
			break;
		}
    tag = seek.ReadTag();
	}
	return true;
}
Line::Line() {
	function_id = 0;
	line = 0;
}
void Line::set_function_id(uint64_t function_id) {
	this->function_id = function_id;
}
void Line::set_line(int64_t line) {
	this->line = line;
}
bool Line::parseLine(Seeker& seek) {
	uint32_t tag = seek.ReadTag();
	while (tag != 0) {
		uint32_t field_id = tag >> 3;
		uint32_t wire = tag & 7;

		switch (field_id) {
		case 1:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_function_id(i);
			}
			break;
		case 2:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_line(i);
			}
			break;
		}
    tag = seek.ReadTag();
	}
	return true;
}
Function::Function() {
	id = 0;
	name = 0;
	system_name = 0;
	filename = 0;
	start_line = 0;
}
void Function::set_id(uint64_t id) {
	this->id = id;
}
void Function::set_name(int64_t name) {
	this->name = name;
}
void Function::set_system_name(int64_t system_name) {
	this->system_name = system_name;
}
void Function::set_filename(int64_t filename) {
	this->filename = filename;
}
void Function::set_start_line(int64_t start_line) {
	this->start_line = start_line;
}
bool Function::parseFunction(Seeker& seek) {
	uint32_t tag = seek.ReadTag();
	while (tag != 0) {
		uint32_t field_id = tag >> 3;
		uint32_t wire = tag & 7;

		switch (field_id) {
		case 1:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_id(i);
			}
			break;
		case 2:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_name(i);
			}
			break;
		case 3:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_system_name(i);
			}
			break;
		case 4:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_filename(i);
			}
			break;
		case 5:
			if (wire == 0) {
				uint64_t i;
				seek.ReadVarint64(&i);
				set_start_line(i);
			}
			break;
		}
    tag = seek.ReadTag();
	}
	return true;
}
