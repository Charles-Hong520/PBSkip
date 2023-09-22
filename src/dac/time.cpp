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
#include <string>
#include <vector>

#include "../dac/profile.pbs.h"
#include "../schema/profile.pb.h"
/*
g++ -std=c++17 src/dac/profile.pbs.cpp src/schema/profile.pb.cc src/dac/time.cpp -lprotobuf -lpthread -O3 -o src/dac/testprofile && src/dac/testprofile
*/
int main() {
    int PBS_NUM_THREADS = 2;
    if (const auto env_p = std::getenv("PBS_NUM_THREADS")) {
        PBS_NUM_THREADS = std::stoi(env_p);
    }
    // std::cout << "threads: " << PBS_NUM_THREADS << " \n";
    // std::cout << "---------------------Running the parallel version----------------------\n";
    std::string file = "dataset/zz.prof";
    Buffer content(file);

    std::chrono::time_point<std::chrono::system_clock> start_T, end_T, start2, end2;
    std::chrono::duration<double> elapsed_seconds, e2;
    //-----------------------------------

    // parses our PBSkip object

    // std::cout << "Parsing " << file << " with custom pthreads" << std::endl;
    // std::cout << "content size: " << content.size << std::endl;

    Seeker seeker(content, 0, content.size);

    PBS::Profile* pbs = new PBS::Profile();
    pbs->fillTracker(seeker);
    auto tr = pbs->getTracker();
    std::map<uint64_t, uint64_t> pos_to_fi;

    const int startCheck = content.size / 2;
    const int endCheck = content.size - 1;
    std::set<int> st;

    int z = 0;
    for (const auto& [field_id, vpos] : tr) {
        // std::cout << "field_id: " << field_id << ", size: " << vpos.size() << std::endl;
        for (auto& [pos, len] : vpos) {
            pos_to_fi[pos] = len;
            if (startCheck <= pos && pos <= endCheck) {
                st.insert(pos);
                // if (z++ < 10) print("good:", pos, "fi:", field_id);
            }
        }
    }
    start_T = std::chrono::system_clock::now();

    auto lf = [&]() {
        Seeker seeker(content, 0, content.size);
        seeker.curr = 0;
        seeker.end = 542006431;
        std::chrono::time_point<std::chrono::system_clock> start_T = std::chrono::system_clock::now();
        if (!pbs->parseProfile(seeker)) {
            std::cout << "false\n";
        }
        std::chrono::time_point<std::chrono::system_clock> end_T = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end_T - start_T;
        std::cout << "time1: " << elapsed_seconds.count() << "s\n";
    };

    auto rf = [&]() {
        Seeker seeker(content, 0, content.size);
        std::chrono::time_point<std::chrono::system_clock> start_T = std::chrono::system_clock::now();
        for (int i = startCheck; i <= endCheck; i++) {
            seeker.curr = i;
            seeker.end = content.size;
            if (pbs->checkValidStart(seeker)) {
                if (st.count(i) == 0) {
                    print("invalid at", i);
                    uint32_t tag = seeker.ReadTag();
                    print(tag >> 3, tag & 7);
                } else {
                    // print("valid at", i);
                    break;
                }
            }
        }
        std::chrono::time_point<std::chrono::system_clock> end_T = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end_T - start_T;
        std::cout << "time2: " << elapsed_seconds.count() << "s\n";
    };

    parlay::par_do(lf, rf);
    start2 = std::chrono::system_clock::now();
    pbs->MergeParallel();
    end2 = std::chrono::system_clock::now();
    end_T = std::chrono::system_clock::now();
    e2 = end2 - start2;
    elapsed_seconds = end_T - start_T;
    print("merge:", e2.count(), "time:", elapsed_seconds.count(), "s");

    // parses google protobuf message object

    // #define pp perftools::profiles
    //     pp::Profile* profile_correct = new pp::Profile();
    //     pp::Profile* profile_custom = new pp::Profile();

    //     //-----------------------------------
    //     // parses our object into the google message object to verify correctness
    //     std::cout << "Converting custom into google message" << std::endl;
    //     for (auto e0 : pbs->get_sample_type()) {
    //         pp::ValueType* temp0 = profile_custom->add_sample_type();
    //         temp0->set_type(e0->get_type());
    //         temp0->set_unit(e0->get_unit());
    //     }
    //     for (auto e0 : pbs->get_sample()) {
    //         pp::Sample* temp0 = profile_custom->add_sample();
    //         for (auto e1 : e0->get_location_id()) {
    //             temp0->add_location_id(e1);
    //         }
    //         for (auto e1 : e0->get_value()) {
    //             temp0->add_value(e1);
    //         }
    //         for (auto e1 : e0->get_label()) {
    //             pp::Label* temp1 = temp0->add_label();
    //             temp1->set_key(e1->get_key());
    //             temp1->set_str(e1->get_str());
    //             temp1->set_num(e1->get_num());
    //             temp1->set_num_unit(e1->get_num_unit());
    //         }
    //     }
    //     for (auto e0 : pbs->get_mapping()) {
    //         pp::Mapping* temp0 = profile_custom->add_mapping();
    //         temp0->set_id(e0->get_id());
    //         temp0->set_memory_start(e0->get_memory_start());
    //         temp0->set_memory_limit(e0->get_memory_limit());
    //         temp0->set_file_offset(e0->get_file_offset());
    //         temp0->set_filename(e0->get_filename());
    //         temp0->set_build_id(e0->get_build_id());
    //         temp0->set_has_functions(e0->get_has_functions());
    //         temp0->set_has_filenames(e0->get_has_filenames());
    //         temp0->set_has_line_numbers(e0->get_has_line_numbers());
    //         temp0->set_has_inline_frames(e0->get_has_inline_frames());
    //     }
    //     for (auto e0 : pbs->get_location()) {
    //         pp::Location* temp0 = profile_custom->add_location();
    //         temp0->set_id(e0->get_id());
    //         temp0->set_mapping_id(e0->get_mapping_id());
    //         temp0->set_address(e0->get_address());
    //         for (auto e1 : e0->get_line()) {
    //             pp::Line* temp1 = temp0->add_line();
    //             temp1->set_function_id(e1->get_function_id());
    //             temp1->set_line(e1->get_line());
    //         }
    //         temp0->set_is_folded(e0->get_is_folded());
    //     }
    //     for (auto e0 : pbs->get_function()) {
    //         pp::Function* temp0 = profile_custom->add_function();
    //         temp0->set_id(e0->get_id());
    //         temp0->set_name(e0->get_name());
    //         temp0->set_system_name(e0->get_system_name());
    //         temp0->set_filename(e0->get_filename());
    //         temp0->set_start_line(e0->get_start_line());
    //     }
    //     for (auto e0 : pbs->get_string_table()) {
    //         profile_custom->add_string_table(e0);
    //     }
    //     profile_custom->set_drop_frames(pbs->get_drop_frames());
    //     profile_custom->set_keep_frames(pbs->get_keep_frames());
    //     profile_custom->set_time_nanos(pbs->get_time_nanos());
    //     profile_custom->set_duration_nanos(pbs->get_duration_nanos());
    //     pp::ValueType* temp0 = new pp::ValueType();
    //     temp0->set_type(pbs->get_period_type()->get_type());
    //     temp0->set_unit(pbs->get_period_type()->get_unit());

    //     profile_custom->set_allocated_period_type(temp0);
    //     profile_custom->set_period(pbs->get_period());
    //     for (auto e0 : pbs->get_comment()) {
    //         profile_custom->add_comment(e0);
    //     }
    //     profile_custom->set_default_sample_type(pbs->get_default_sample_type());

    //     delete pbs;

    //     std::cout << "Parsing " << file << " with Google API" << std::endl;
    //     std::fstream cinput(file, std::ios::in | std::ios::binary);

    //     start_T = std::chrono::system_clock::now();
    //     profile_correct->ParseFromIstream(&cinput);
    //     end_T = std::chrono::system_clock::now();
    //     elapsed_seconds = end_T - start_T;

    //     std::cout << "Time for Google API parse: " << elapsed_seconds.count() << "s\n";

    //     //-----------------------------------
    //     if (!google::protobuf::util::MessageDifferencer::Equivalent(*profile_custom, *profile_correct)) {
    //         std::cout << "MISMATCH" << std::endl;
    //         int64_t m1 = profile_correct->ByteSizeLong();
    //         int64_t m2 = profile_custom->ByteSizeLong();
    //         print("correct size:", m1);
    //         print("custom size:", m2);

    //     } else {
    //         std::cout << "MATCH" << std::endl;
    //     }
}