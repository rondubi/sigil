#!/bin/bash

sigil_file=$1
echo "Sigil file: $sigil_file"

sigil_file_stem=$sigil_file
sigil_file_stem="${sigil_file_stem##*/}"
sigil_file_stem="${sigil_file_stem%.sigil}"
echo "Sigil file stem: $sigil_file_stem"

sigil_gend_cc="compile/sigil_gend_$sigil_file_stem.cc"
echo "Sigil generated cc file at $sigil_gend_cc"

cd frontend
make
cd ..
echo "Compiled parser"

cd backend
make
cd ..
echo "Compiled Sigil backend"

touch $sigil_gend_cc
./frontend/build/parser $sigil_file > $sigil_gend_cc
echo "Generated C++"

sigil_executable="compile/sigil_exec_$sigil_file_stem"
echo "Executable will be at $sigil_executable"

sigil_obj_file="$sigil_executable.o"

clang++ -c -Wall -Wpedantic -std=c++2a -I backend $sigil_gend_cc -o $sigil_obj_file
echo "Compiled object file"

clang++ $sigil_obj_file backend/build/sigil.o -o $sigil_executable
echo "Compiled executable"

