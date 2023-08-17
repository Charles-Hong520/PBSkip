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

#include "../parallel/profile.pbs.h"
#include "../schema/profile.pb.h"
// #include <google/protobuf/port_undef.inc>

/*
g++ -std=c++17 src/schema/profile.pb.cc src/google_api/testprofile.cpp -lprotobuf -lpthread -O3 -o src/google_api/testprofile && src/google_api/testprofile
*/

#define pp perftools::profiles

int main() {
    std::cout << "---------------------Running the Google API version----------------------\n";
    std::string file = "dataset/zz.prof";
    pp::Profile* profile_correct = new pp::Profile();

    std::chrono::time_point<std::chrono::system_clock> start_T, end_T;
    std::chrono::duration<double> elapsed_seconds[5];
    //-----------------------------------

    // parses google protobuf message object

    std::cout << "Parsing " << file << " with Google API" << std::endl;
    std::fstream cinput(file, std::ios::in | std::ios::binary);

    for (int i = 0; i < 5; i++) {
        start_T = std::chrono::system_clock::now();
        profile_correct->ParseFromIstream(&cinput);
        end_T = std::chrono::system_clock::now();
        elapsed_seconds[i] = end_T - start_T;
        profile_correct->Clear();
    }

    std::cout << "Time for Google API parse:\n";
    for (int i = 0; i < 5; i++) {
        std::cout << "attempt " << i + 1 << " " << elapsed_seconds[i].count() << "s\n";
    }
}
