#ifndef __PROFILE_PBSTOPB_H__
#define __PROFILE_PBSTOPB_H__
for (auto e : pbs.get_sample_type()) {
ValueType* temp = profile_custom.add_sample_type();
temp->set_type(e.get_type());
temp->set_unit(e.get_unit());
}
for (auto e : pbs.get_sample()) {
Sample* temp = profile_custom.add_sample();
for (auto e : e.get_location_id()) {
temp->add_location_id(e);
}
for (auto e : e.get_value()) {
temp->add_value(e);
}
for (auto e : e.get_label()) {
Label* temp = temp.add_label();
temp->set_key(e.get_key());
temp->set_str(e.get_str());
temp->set_num(e.get_num());
temp->set_num_unit(e.get_num_unit());
}
}
for (auto e : pbs.get_mapping()) {
Mapping* temp = profile_custom.add_mapping();
temp->set_id(e.get_id());
temp->set_memory_start(e.get_memory_start());
temp->set_memory_limit(e.get_memory_limit());
temp->set_file_offset(e.get_file_offset());
temp->set_filename(e.get_filename());
temp->set_build_id(e.get_build_id());
temp->set_has_functions(e.get_has_functions());
temp->set_has_filenames(e.get_has_filenames());
temp->set_has_line_numbers(e.get_has_line_numbers());
temp->set_has_inline_frames(e.get_has_inline_frames());
}
for (auto e : pbs.get_location()) {
Location* temp = profile_custom.add_location();
temp->set_id(e.get_id());
temp->set_mapping_id(e.get_mapping_id());
temp->set_address(e.get_address());
for (auto e : e.get_line()) {
Line* temp = temp.add_line();
temp->set_function_id(e.get_function_id());
temp->set_line(e.get_line());
}
temp->set_is_folded(e.get_is_folded());
}
for (auto e : pbs.get_function()) {
Function* temp = profile_custom.add_function();
temp->set_id(e.get_id());
temp->set_name(e.get_name());
temp->set_system_name(e.get_system_name());
temp->set_filename(e.get_filename());
temp->set_start_line(e.get_start_line());
}
for (auto e : pbs.get_string_table()) {
profile_custom->add_string_table(e);
}
profile_custom->set_drop_frames(pbs.get_drop_frames());
profile_custom->set_keep_frames(pbs.get_keep_frames());
profile_custom->set_time_nanos(pbs.get_time_nanos());
profile_custom->set_duration_nanos(pbs.get_duration_nanos());
ValueType* temp = new ValueType();
temp->set_type(pbs.get_period_type().get_type());
temp->set_unit(pbs.get_period_type().get_unit());

ValueType* temp = profile_custom.set_allocated_period_type();
profile_custom->set_period(pbs.get_period());
for (auto e : pbs.get_comment()) {
profile_custom->add_comment(e);
}
profile_custom->set_default_sample_type(pbs.get_default_sample_type());


#endif
