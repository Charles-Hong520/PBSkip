#include "profile.pbs.h"
namespace PBS {
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
std::vector<ValueType*> Profile::get_sample_type() {
    return this->sample_type;
}
void Profile::add_sample(Sample* sample) {
    this->sample.push_back(sample);
}
std::vector<Sample*> Profile::get_sample() {
    return this->sample;
}
void Profile::add_mapping(Mapping* mapping) {
    this->mapping.push_back(mapping);
}
std::vector<Mapping*> Profile::get_mapping() {
    return this->mapping;
}
void Profile::add_location(Location* location) {
    this->location.push_back(location);
}
std::vector<Location*> Profile::get_location() {
    return this->location;
}
void Profile::add_function(Function* function) {
    this->function.push_back(function);
}
std::vector<Function*> Profile::get_function() {
    return this->function;
}
void Profile::add_string_table(const std::string& string_table) {
    this->string_table.push_back(string_table);
}
std::vector<std::string> Profile::get_string_table() {
    return this->string_table;
}
void Profile::set_drop_frames(int64_t drop_frames) {
    this->drop_frames = drop_frames;
}
int64_t Profile::get_drop_frames() {
    return this->drop_frames;
}
void Profile::set_keep_frames(int64_t keep_frames) {
    this->keep_frames = keep_frames;
}
int64_t Profile::get_keep_frames() {
    return this->keep_frames;
}
void Profile::set_time_nanos(int64_t time_nanos) {
    this->time_nanos = time_nanos;
}
int64_t Profile::get_time_nanos() {
    return this->time_nanos;
}
void Profile::set_duration_nanos(int64_t duration_nanos) {
    this->duration_nanos = duration_nanos;
}
int64_t Profile::get_duration_nanos() {
    return this->duration_nanos;
}
void Profile::set_period_type(ValueType* period_type) {
    this->period_type = period_type;
}
ValueType* Profile::get_period_type() {
    return this->period_type;
}
void Profile::set_period(int64_t period) {
    this->period = period;
}
int64_t Profile::get_period() {
    return this->period;
}
void Profile::add_comment(int64_t comment) {
    this->comment.push_back(comment);
}
std::vector<int64_t> Profile::get_comment() {
    return this->comment;
}
void Profile::set_default_sample_type(int64_t default_sample_type) {
    this->default_sample_type = default_sample_type;
}
int64_t Profile::get_default_sample_type() {
    return this->default_sample_type;
}
bool Profile::parseProfile(Seeker& seek) {
    uint32_t tag = seek.ReadTag();
    uint32_t field_id, wire, len;
    while (tag != 0) {
        field_id = tag >> 3;
        wire = tag & 7;
        uint64_t i;
        if (wire == 0) {
            seek.ReadVarint64(&i);
            switch (field_id) {
                case 7:
                    //  int64 drop_frames = 7;
                    set_drop_frames(i);
                    break;
                case 8:
                    //  int64 keep_frames = 8;
                    set_keep_frames(i);
                    break;
                case 9:
                    //  int64 time_nanos = 9;
                    set_time_nanos(i);
                    break;
                case 10:
                    //  int64 duration_nanos = 10;
                    set_duration_nanos(i);
                    break;
                case 12:
                    //  int64 period = 12;
                    set_period(i);
                    break;
                case 13:
                    //  repeated int64 comment = 13;
                    add_comment(i);
                    break;
                case 14:

                    //  int64 default_sample_type = 14;
                    set_default_sample_type(i);
                    break;
            }
        } else if (wire == 2) {
            if (field_id == 11) {
                seek.ReadVarint32(&len);
                ValueType* msg_ = new ValueType();
                Seeker copyseeker(seek, len);
                seek.Skip(len);
                msg_->parseValueType(copyseeker);
                set_period_type(msg_);
            } else {
                if (!seek.ReadVarint32(&len)) {
                    handle_error("read len 32 wrong" + std::to_string(seek.curr));
                }
                tracker[field_id].push_back({seek.curr, len});
                seek.Skip(len);
            }
        }
        tag = seek.ReadTag();
    }

    sample_type.resize(tracker[1].size());
    sample.resize(tracker[2].size());
    mapping.resize(tracker[3].size());
    location.resize(tracker[4].size());
    function.resize(tracker[5].size());
    string_table.resize(tracker[6].size());
    comment.resize(tracker[13].size());
    for (auto& e : sample) e = new Sample();
    // for (const auto& [fid, v] : tracker) {
    //   int sum = 0;
    //   for (auto e : v) sum += e.second;

    //   print(fid, v.size(), sum);
    // }

    bulk_add_sample_type(tracker[1], seek.buffer);
    bulk_add_sample(tracker[2], seek.buffer);
    bulk_add_mapping(tracker[3], seek.buffer);
    bulk_add_location(tracker[4], seek.buffer);
    bulk_add_function(tracker[5], seek.buffer);
    bulk_add_string_table(tracker[6], seek.buffer);
    bulk_add_comment(tracker[13], seek.buffer);
    // print("done parsing profile inside parseProfile()");
    return true;
}

// destructor for Profile for deleting all pointer and preventing memory leak
Profile::~Profile() {
    for (auto e : sample_type) delete e;
    for (auto e : sample) delete e;
    for (auto e : mapping) delete e;
    for (auto e : location) delete e;
    for (auto e : function) delete e;
}

ValueType::ValueType() {
    type = 0;
    unit = 0;
}
void ValueType::set_type(int64_t type) {
    this->type = type;
}
int64_t ValueType::get_type() {
    return this->type;
}
void ValueType::set_unit(int64_t unit) {
    this->unit = unit;
}
int64_t ValueType::get_unit() {
    return this->unit;
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
        // print("ValueType",seek.curr, field_id);
        tag = seek.ReadTag();
    }
    return true;
}
Sample::Sample() {
}
void Sample::add_location_id(uint64_t location_id) {
    this->location_id.push_back(location_id);
}
std::vector<uint64_t> Sample::get_location_id() {
    return this->location_id;
}
void Sample::add_value(int64_t value) {
    this->value.push_back(value);
}
std::vector<int64_t> Sample::get_value() {
    return this->value;
}
void Sample::add_label(Label* label) {
    this->label.push_back(label);
}
std::vector<Label*> Sample::get_label() {
    return this->label;
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
                    uint32_t endOfMsg = seek.curr + len;
                    while (seek.curr < endOfMsg) {
                        seek.ReadVarint64(&i);
                        add_location_id(i);
                    }
                } else if (wire == 0) {
                    uint64_t i;
                    seek.ReadVarint64(&i);
                    add_location_id(i);
                }
                break;
            case 2:
                if (wire == 2) {
                    uint32_t len;
                    seek.ReadVarint32(&len);
                    uint64_t i;
                    uint32_t endOfMsg = seek.curr + len;
                    while (seek.curr < endOfMsg) {
                        seek.ReadVarint64(&i);
                        add_value(i);
                    }
                } else if (wire == 0) {
                    uint64_t i;
                    seek.ReadVarint64(&i);
                    add_value(i);
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
        // print("Sample",seek.curr, field_id);
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
int64_t Label::get_key() {
    return this->key;
}
void Label::set_str(int64_t str) {
    this->str = str;
}
int64_t Label::get_str() {
    return this->str;
}
void Label::set_num(int64_t num) {
    this->num = num;
}
int64_t Label::get_num() {
    return this->num;
}
void Label::set_num_unit(int64_t num_unit) {
    this->num_unit = num_unit;
}
int64_t Label::get_num_unit() {
    return this->num_unit;
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
        // print("Label",seek.curr, field_id);
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
uint64_t Mapping::get_id() {
    return this->id;
}
void Mapping::set_memory_start(uint64_t memory_start) {
    this->memory_start = memory_start;
}
uint64_t Mapping::get_memory_start() {
    return this->memory_start;
}
void Mapping::set_memory_limit(uint64_t memory_limit) {
    this->memory_limit = memory_limit;
}
uint64_t Mapping::get_memory_limit() {
    return this->memory_limit;
}
void Mapping::set_file_offset(uint64_t file_offset) {
    this->file_offset = file_offset;
}
uint64_t Mapping::get_file_offset() {
    return this->file_offset;
}
void Mapping::set_filename(int64_t filename) {
    this->filename = filename;
}
int64_t Mapping::get_filename() {
    return this->filename;
}
void Mapping::set_build_id(int64_t build_id) {
    this->build_id = build_id;
}
int64_t Mapping::get_build_id() {
    return this->build_id;
}
void Mapping::set_has_functions(bool has_functions) {
    this->has_functions = has_functions;
}
bool Mapping::get_has_functions() {
    return this->has_functions;
}
void Mapping::set_has_filenames(bool has_filenames) {
    this->has_filenames = has_filenames;
}
bool Mapping::get_has_filenames() {
    return this->has_filenames;
}
void Mapping::set_has_line_numbers(bool has_line_numbers) {
    this->has_line_numbers = has_line_numbers;
}
bool Mapping::get_has_line_numbers() {
    return this->has_line_numbers;
}
void Mapping::set_has_inline_frames(bool has_inline_frames) {
    this->has_inline_frames = has_inline_frames;
}
bool Mapping::get_has_inline_frames() {
    return this->has_inline_frames;
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
        // print("Mapping",seek.curr, field_id);
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
uint64_t Location::get_id() {
    return this->id;
}
void Location::set_mapping_id(uint64_t mapping_id) {
    this->mapping_id = mapping_id;
}
uint64_t Location::get_mapping_id() {
    return this->mapping_id;
}
void Location::set_address(uint64_t address) {
    this->address = address;
}
uint64_t Location::get_address() {
    return this->address;
}
void Location::add_line(Line* line) {
    this->line.push_back(line);
}
std::vector<Line*> Location::get_line() {
    return this->line;
}
void Location::set_is_folded(bool is_folded) {
    this->is_folded = is_folded;
}
bool Location::get_is_folded() {
    return this->is_folded;
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
        // print("Location",seek.curr, field_id);
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
uint64_t Line::get_function_id() {
    return this->function_id;
}
void Line::set_line(int64_t line) {
    this->line = line;
}
int64_t Line::get_line() {
    return this->line;
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
        // print("Line",seek.curr, field_id);
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
uint64_t Function::get_id() {
    return this->id;
}
void Function::set_name(int64_t name) {
    this->name = name;
}
int64_t Function::get_name() {
    return this->name;
}
void Function::set_system_name(int64_t system_name) {
    this->system_name = system_name;
}
int64_t Function::get_system_name() {
    return this->system_name;
}
void Function::set_filename(int64_t filename) {
    this->filename = filename;
}
int64_t Function::get_filename() {
    return this->filename;
}
void Function::set_start_line(int64_t start_line) {
    this->start_line = start_line;
}
int64_t Function::get_start_line() {
    return this->start_line;
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
        // print("Function",seek.curr, field_id);
        tag = seek.ReadTag();
    }
    return true;
}

void Profile::bulk_add_sample_type(std::vector<std::pair<uint64_t, uint64_t>>& tr, uint8_t* b) {
    parlay::parallel_for(0, tr.size(), [&](int i) {
        Seeker seeker(b, tr[i].first, tr[i].second);
        sample_type[i] = new ValueType();
        sample_type[i]->parseValueType(seeker);
    });
}

void Profile::bulk_add_sample(std::vector<std::pair<uint64_t, uint64_t>>& tr, uint8_t* b) {
    parlay::parallel_for(0, tr.size(), [&](int i) {
        Seeker seeker(b, tr[i].first, tr[i].second);
        // sample[i] = new Sample();
        sample[i]->parseSample(seeker);
    });
}

void Profile::bulk_add_mapping(std::vector<std::pair<uint64_t, uint64_t>>& tr, uint8_t* b) {
    parlay::parallel_for(0, tr.size(), [&](int i) {
        Seeker seeker(b, tr[i].first, tr[i].second);
        mapping[i] = new Mapping();
        mapping[i]->parseMapping(seeker);
    });
}

void Profile::bulk_add_location(std::vector<std::pair<uint64_t, uint64_t>>& tr, uint8_t* b) {
    parlay::parallel_for(0, tr.size(), [&](int i) {
        Seeker seeker(b, tr[i].first, tr[i].second);
        location[i] = new Location();
        location[i]->parseLocation(seeker);
    });
}

void Profile::bulk_add_function(std::vector<std::pair<uint64_t, uint64_t>>& tr, uint8_t* b) {
    parlay::parallel_for(0, tr.size(), [&](int i) {
        Seeker seeker(b, tr[i].first, tr[i].second);
        function[i] = new Function();
        function[i]->parseFunction(seeker);
    });
}

void Profile::bulk_add_string_table(std::vector<std::pair<uint64_t, uint64_t>>& tr, uint8_t* b) {
    parlay::parallel_for(0, tr.size(), [&](int i) {
        Seeker seeker(b, tr[i].first, tr[i].second);
        seeker.ReadString(&string_table[i], tr[i].second);
    });
}

void Profile::bulk_add_comment(std::vector<std::pair<uint64_t, uint64_t>>& tr, uint8_t* b) {
    parlay::parallel_for(0, tr.size(), [&](int i) {
        Seeker seeker(b, tr[i].first, tr[i].second);
        uint64_t i64;
        while (seeker.curr < seeker.end) {
            seeker.ReadVarint64(&i64);
            add_comment(i64);
        }
    });
}
}  // namespace PBS
