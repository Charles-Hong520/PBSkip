#!/bin/bash -l
protoc --proto_path=src/schema/ --cpp_out=src/schema/ profile.proto
g++ -std=c++17 src/pthreads/profile.pbs.cpp src/schema/profile.pb.cc src/pthreads/testprofile.cpp -lprotobuf -lpthread -O3 -o src/pthreads/testprofile
g++ -std=c++17 src/sequential/profile.pbs.cpp src/schema/profile.pb.cc src/sequential/testprofile.cpp -lprotobuf -lpthread -O3 -o src/sequential/testprofile
g++ -std=c++17 src/parallel/profile.pbs.cpp src/schema/profile.pb.cc src/parallel/testprofile.cpp -lprotobuf -lpthread -O3 -o src/parallel/testprofile
g++ -std=c++17 src/schema/profile.pb.cc src/google_api/testprofile.cpp -lprotobuf -lpthread -O3 -o src/google_api/testprofile
g++ -std=c++17 src/dac/profile.pbs.cpp src/schema/profile.pb.cc src/dac/time.cpp -lprotobuf -lpthread -O3 -o src/dac/testprofile
# export PBS_NUM_THREADS=2
# echo "------------------Running the 2 pthreads version-------------------"
# for i in {1..5}; do src/pthreads/testprofile; done
# export PBS_NUM_THREADS=4
# echo "------------------Running the 4 pthreads version-------------------"
# for i in {1..5}; do src/pthreads/testprofile; done
# export PBS_NUM_THREADS=8
# echo "------------------Running the 8 pthreads version-------------------"
# for i in {1..5}; do src/pthreads/testprofile; done
# export PBS_NUM_THREADS=16
# echo "------------------Running the 16 pthreads version------------------"
# for i in {1..5}; do src/pthreads/testprofile; done
# export PBS_NUM_THREADS=32
# echo "------------------Running the 32 pthreads version------------------"
# for i in {1..5}; do src/pthreads/testprofile; done
export PBS_NUM_THREADS=64
echo "------------------Running the 64 pthreads version------------------"
for i in {1..5}; do src/pthreads/testprofile; done
echo "------------------Running the Sequential verison-------------------"
for i in {1..5}; do src/sequential/testprofile; done
echo "------------------Running the Parallel verison---------------------"
for i in {1..5}; do src/parallel/testprofile; done
echo "------------------Running the Google API verison-------------------"
for i in {1..5}; do src/google_api/testprofile; done
echo "------------------Running the DAC verison-------------------"
for i in {1..5}; do src/dac/testprofile; done