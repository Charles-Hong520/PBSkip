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
    std::cout << "threads: " << PBS_NUM_THREADS << " \n";
    // std::cout << "---------------------Running the parallel version----------------------\n";
    std::string file = "dataset/zz.prof";
    Buffer content(file);

    std::chrono::time_point<std::chrono::system_clock> start_T, end_T;
    std::chrono::duration<double> elapsed_seconds;
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

    print(startCheck);

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
                    print("valid at", i);
                    break;
                }
            }
        }
        std::chrono::time_point<std::chrono::system_clock> end_T = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end_T - start_T;
        std::cout << "time2: " << elapsed_seconds.count() << "s\n";
    };

    parlay::par_do(lf, rf);
    end_T = std::chrono::system_clock::now();
    elapsed_seconds = end_T - start_T;
    std::cout << "timetot: " << elapsed_seconds.count() << "s\n";
}