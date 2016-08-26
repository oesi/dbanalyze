#!/bin/bash

# Export Lib Path
export LD_LIBRARY_PATH=.export LD_LIBRARY_PATH=./lib/bin

echo '' > compilerlog

#### Compile LIB ####

# get cc files from lib/src/
lib_src="`ls -d -1 ./lib/src/**.cc`"

# compile to shared library
g++ -shared $lib_src \
	-std=gnu++11 `pkg-config --cflags --libs libgda-5.0` \
	-fPIC \
	-I ./lib/include \
	-o ./lib/bin/libdatabase.so &>> compilerlog

# Save Return Value
ret_lib=$?

#### Compile CLI ####

# get cc files from cli/src/
cli_src="`ls -d -1 ./cli/src/**.cc`"

# compile commandline interface
g++ $cli_src -std=gnu++11 `pkg-config --cflags --libs libgda-5.0` \
	-L ./lib/bin \
	-I ./cli/include \
	-I ./lib/include \
	-lboost_program_options\
	-ldatabase \
	-o ./cli/bin/dbanalyze_cli &>> compilerlog

# Save Return Value
ret_cli=$?

# Start program or display error messages
if [[ ( $ret_cli -eq 0 ) && ( $ret_lib -eq 0 ) ]]; then
	./cli/bin/dbanalyze_cli -h localhost -p 5433 -u dbanalyze -d analyzetest -t PostgreSQL -w dbanalyze
else
	lines="`cat compilerlog | wc -l`"
	echo '========== Compilation Failed =========';
	if [[ $lines -gt 20 ]]; then
		less compilerlog
	else
		cat compilerlog
	fi
fi
