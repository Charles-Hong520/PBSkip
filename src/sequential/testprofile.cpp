#include <fcntl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/message_differencer.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "../generated/profile.pbs.h"
#include "../schema/profile.pb.h"
// #include <google/protobuf/port_undef.inc>

/*
g++ -std=c++17 src/generated/profile.pbs.cpp src/schema/profile.pb.cc src/sequential/testprofile.cpp -lprotobuf -lpthread -O3 -o src/sequential/testprofile && ./src/sequential/testprofile
*/

#define pp perftools::profiles
int main() {
  std::string file = "dataset/zz.prof";
  Buffer content(file);
  PBS::Profile* pbs = new PBS::Profile();
  pp::Profile* profile_correct = new pp::Profile();
  pp::Profile* profile_custom = new pp::Profile();

  std::chrono::time_point<std::chrono::system_clock> start_T, end_T;
  std::chrono::duration<double> elapsed_seconds;
  //-----------------------------------

  // parses our PBSkip object

  std::cout << "Parsing " << file << " with custom API" << std::endl;
  std::cout << "content size: " << content.size << std::endl;
  Seeker seeker(content, 0, content.size);
  pbs->parseProfile(seeker);

  //-----------------------------------

  // parses google protobuf message object

  std::cout << "Parsing " << file << " with Google API" << std::endl;
  std::fstream cinput(file, std::ios::in | std::ios::binary);
  profile_correct->ParseFromIstream(&cinput);

  // google::protobuf::CodedInputStream* input = new google::protobuf::CodedInputStream(content.buffer, content.size);
  // profile_correct.ParseFromCodedStream(input);

  // std::string s_correct;
  // parser.profile_correct.SerializeToString(&s_correct);
  // cout << "size correct: " << s_correct.size() << endl;

  //-----------------------------------
  // parses our object into the google message object to verify correctness
  std::cout << "Converting custom into google message" << std::endl;
  for (auto e0 : pbs->get_sample_type()) {
    pp::ValueType* temp0 = profile_custom->add_sample_type();
    temp0->set_type(e0->get_type());
    temp0->set_unit(e0->get_unit());
  }
  for (auto e0 : pbs->get_sample()) {
    pp::Sample* temp0 = profile_custom->add_sample();
    for (auto e1 : e0->get_location_id()) {
      temp0->add_location_id(e1);
    }
    for (auto e1 : e0->get_value()) {
      temp0->add_value(e1);
    }
    for (auto e1 : e0->get_label()) {
      pp::Label* temp1 = temp0->add_label();
      temp1->set_key(e1->get_key());
      temp1->set_str(e1->get_str());
      temp1->set_num(e1->get_num());
      temp1->set_num_unit(e1->get_num_unit());
    }
  }
  for (auto e0 : pbs->get_mapping()) {
    pp::Mapping* temp0 = profile_custom->add_mapping();
    temp0->set_id(e0->get_id());
    temp0->set_memory_start(e0->get_memory_start());
    temp0->set_memory_limit(e0->get_memory_limit());
    temp0->set_file_offset(e0->get_file_offset());
    temp0->set_filename(e0->get_filename());
    temp0->set_build_id(e0->get_build_id());
    temp0->set_has_functions(e0->get_has_functions());
    temp0->set_has_filenames(e0->get_has_filenames());
    temp0->set_has_line_numbers(e0->get_has_line_numbers());
    temp0->set_has_inline_frames(e0->get_has_inline_frames());
  }
  for (auto e0 : pbs->get_location()) {
    pp::Location* temp0 = profile_custom->add_location();
    temp0->set_id(e0->get_id());
    temp0->set_mapping_id(e0->get_mapping_id());
    temp0->set_address(e0->get_address());
    for (auto e1 : e0->get_line()) {
      pp::Line* temp1 = temp0->add_line();
      temp1->set_function_id(e1->get_function_id());
      temp1->set_line(e1->get_line());
    }
    temp0->set_is_folded(e0->get_is_folded());
  }
  for (auto e0 : pbs->get_function()) {
    pp::Function* temp0 = profile_custom->add_function();
    temp0->set_id(e0->get_id());
    temp0->set_name(e0->get_name());
    temp0->set_system_name(e0->get_system_name());
    temp0->set_filename(e0->get_filename());
    temp0->set_start_line(e0->get_start_line());
  }
  for (auto e0 : pbs->get_string_table()) {
    profile_custom->add_string_table(e0);
  }
  profile_custom->set_drop_frames(pbs->get_drop_frames());
  profile_custom->set_keep_frames(pbs->get_keep_frames());
  profile_custom->set_time_nanos(pbs->get_time_nanos());
  profile_custom->set_duration_nanos(pbs->get_duration_nanos());
  pp::ValueType* temp0 = new pp::ValueType();
  temp0->set_type(pbs->get_period_type()->get_type());
  temp0->set_unit(pbs->get_period_type()->get_unit());

  profile_custom->set_allocated_period_type(temp0);
  profile_custom->set_period(pbs->get_period());
  for (auto e0 : pbs->get_comment()) {
    profile_custom->add_comment(e0);
  }
  profile_custom->set_default_sample_type(pbs->get_default_sample_type());

  //-----------------------------------
  if (!google::protobuf::util::MessageDifferencer::Equivalent(*profile_custom, *profile_correct)) {
    std::cout << "MISMATCH" << std::endl;
  } else {
    std::cout << "MATCH" << std::endl;
  }
}