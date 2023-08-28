#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=64
#SBATCH --mem=128G
#SBATCH --time=1-00:15:00     # 1 day and 15 minutes
#SBATCH --mail-user=dparm003@ucr.edu
#SBATCH --mail-type=ALL
#SBATCH --job-name="seq and par parse zz.prof"
#SBATCH -p intel
date
module load cmake
module load gcc
module load protobuf/
protoc --proto_path=src/schema/ --cpp_out=src/schema/ profile.proto
cd src/sequential/
rm -rf build
mkdir build
cd build
cmake ..
make
cd ../../parallel/
rm -rf build
mkdir build
cd build
cmake ..
make
cd ../../google_api/
rm -rf build
mkdir build
cd build
cmake ..
make
cd ../../..
echo "------------------Running the Sequential verison-------------------"
for i in {1..10}; do ./src/sequential/build/ProfileProject; done
echo "------------------Running the Parallel verison---------------------"
for i in {1..10}; do ./src/parallel/build/ProfileProject; done
echo "------------------Running the Google API verison-------------------"
for i in {1..10}; do ./src/google_api/build/ProfileProject; done
hostname
