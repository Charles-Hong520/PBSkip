cmake_minimum_required(VERSION 3.10)

project(ProfileProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED CONFIG)

include_directories(${PROTOBUF_INCLUDE_DIRS})

set(SOURCES
    src/parallel/profile.pbs.cpp
    src/schema/profile.pb.cc
    src/parallel/testprofile.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} protobuf::libprotobuf protobuf::libprotoc Threads::Threads)

