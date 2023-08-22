#ifndef __PROFILE_H__
#define __PROFILE_H__

#include "../lib.h"
#include "parallel.h"
namespace PBS {
class Profile;
class ValueType;
class Sample;
class Label;
class Mapping;
class Location;
class Line;
class Function;
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
    std::map<uint32_t, std::vector<std::pair<uint64_t, uint64_t>>> tracker;

   public:
    Profile();
    void add_sample_type(ValueType*);

    void bulk_add_sample_type(std::vector<std::pair<uint64_t, uint64_t>>&, uint8_t*);
    void bulk_add_sample(std::vector<std::pair<uint64_t, uint64_t>>&, uint8_t*);
    void bulk_add_mapping(std::vector<std::pair<uint64_t, uint64_t>>&, uint8_t*);
    void bulk_add_location(std::vector<std::pair<uint64_t, uint64_t>>&, uint8_t*);
    void bulk_add_function(std::vector<std::pair<uint64_t, uint64_t>>&, uint8_t*);
    void bulk_add_string_table(std::vector<std::pair<uint64_t, uint64_t>>&, uint8_t*);
    void bulk_add_comment(std::vector<std::pair<uint64_t, uint64_t>>&, uint8_t*);

    std::vector<ValueType*> get_sample_type();
    void add_sample(Sample*);
    std::vector<Sample*> get_sample();
    void add_mapping(Mapping*);
    std::vector<Mapping*> get_mapping();
    void add_location(Location*);
    std::vector<Location*> get_location();
    void add_function(Function*);
    std::vector<Function*> get_function();
    void add_string_table(const std::string&);
    std::vector<std::string> get_string_table();
    void set_drop_frames(int64_t);
    int64_t get_drop_frames();
    void set_keep_frames(int64_t);
    int64_t get_keep_frames();
    void set_time_nanos(int64_t);
    int64_t get_time_nanos();
    void set_duration_nanos(int64_t);
    int64_t get_duration_nanos();
    void set_period_type(ValueType*);
    ValueType* get_period_type();
    void set_period(int64_t);
    int64_t get_period();
    void add_comment(int64_t);
    std::vector<int64_t> get_comment();
    void set_default_sample_type(int64_t);
    int64_t get_default_sample_type();
    bool parseProfile(Seeker&);
    friend std::ostream& operator<<(std::ostream& os, const Profile&);
    // make a destruction to prevent memory leaks
    ~Profile();
};
class ValueType {
   private:
    int64_t type;
    int64_t unit;

   public:
    ValueType();
    void set_type(int64_t);
    int64_t get_type();
    void set_unit(int64_t);
    int64_t get_unit();
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
    std::vector<uint64_t> get_location_id();
    void add_value(int64_t);
    std::vector<int64_t> get_value();
    void add_label(Label*);
    std::vector<Label*> get_label();
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
    int64_t get_key();
    void set_str(int64_t);
    int64_t get_str();
    void set_num(int64_t);
    int64_t get_num();
    void set_num_unit(int64_t);
    int64_t get_num_unit();
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
    uint64_t get_id();
    void set_memory_start(uint64_t);
    uint64_t get_memory_start();
    void set_memory_limit(uint64_t);
    uint64_t get_memory_limit();
    void set_file_offset(uint64_t);
    uint64_t get_file_offset();
    void set_filename(int64_t);
    int64_t get_filename();
    void set_build_id(int64_t);
    int64_t get_build_id();
    void set_has_functions(bool);
    bool get_has_functions();
    void set_has_filenames(bool);
    bool get_has_filenames();
    void set_has_line_numbers(bool);
    bool get_has_line_numbers();
    void set_has_inline_frames(bool);
    bool get_has_inline_frames();
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
    uint64_t get_id();
    void set_mapping_id(uint64_t);
    uint64_t get_mapping_id();
    void set_address(uint64_t);
    uint64_t get_address();
    void add_line(Line*);
    std::vector<Line*> get_line();
    void set_is_folded(bool);
    bool get_is_folded();
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
    uint64_t get_function_id();
    void set_line(int64_t);
    int64_t get_line();
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
    uint64_t get_id();
    void set_name(int64_t);
    int64_t get_name();
    void set_system_name(int64_t);
    int64_t get_system_name();
    void set_filename(int64_t);
    int64_t get_filename();
    void set_start_line(int64_t);
    int64_t get_start_line();
    bool parseFunction(Seeker&);
    friend std::ostream& operator<<(std::ostream& os, const Function&);
};
}  // namespace PBS
#endif
