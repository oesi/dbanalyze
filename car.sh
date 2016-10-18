#!/bin/bash

# Export Lib Path
export LD_LIBRARY_PATH=.export LD_LIBRARY_PATH=./lib/bin

echo '' > compilerlog

#### Compile LIB ####
ret_lib=0
if [[ $1 != 'gui' ]]; then
# get cc files from lib/src/
lib_src="`ls -d -1 ./lib/src/**.cc`"

# compile to shared library
g++ -shared $lib_src \
	-std=gnu++11 `pkg-config --cflags --libs libgda-5.0` \
	-fPIC \
	-Wall \
	-lcgraph \
	-lgvc \
	-I ./lib/include \
	-o ./lib/bin/libdbanalyze.so &>> compilerlog

# Save Return Value
ret_lib=$?
fi

#### Compile CLI ####
ret_cli=0
if [[ $1 != 'gui' ]]; then
# get cc files from cli/src/
cli_src="`ls -d -1 ./cli/src/**.cc`"

# compile commandline interface
g++ $cli_src -std=gnu++11 `pkg-config --cflags --libs libgda-5.0` \
	-Wall \
	-L ./lib/bin \
	-I ./cli/include \
	-I ./lib/include \
	-lboost_program_options\
	-ldbanalyze \
	-o ./cli/bin/dbanalyze_cli &>> compilerlog

# Save Return Value
ret_cli=$?
fi
#### Compile GUI ####

# get cc files from gui/src/
gui_src="`ls -d -1 ./gui/src/**.cc`"

# compile commandline interface
g++ $gui_src -std=gnu++11 `pkg-config --cflags --libs gtkmm-3.0 libgda-5.0` \
	-Wall \
	-L ./lib/bin \
	-I ./gui/include \
	-I ./lib/include \
	-ldbanalyze \
	-o ./gui/bin/dbanalyze_gui &>> compilerlog

# Save Return Value
ret_gui=$?

lines="`cat compilerlog | wc -l`"
# Start program or display error messages
#./cli/bin/dbanalyze_cli -h localhost -p 5433 -u dbanalyze -d analyzetest -t PostgreSQL -w dbanalyze -xaa | java -jar /home/oesi/Downloads/plantuml.jar -pipe > diag.png
if [[ ( $ret_cli -eq 0 ) && ( $ret_lib -eq 0 ) && ( $ret_gui -eq 0 ) ]]; then
	if [[ $1 == 'norun' ]]; then
		echo "done"
	elif [[ $1 == 'mysql' ]]; then
		./cli/bin/dbanalyze_cli -h localhost -p 3306 -u root -d dbanalyze -t MySQL
	elif [[ $1 == 'msaccess' ]]; then
		./cli/bin/dbanalyze_cli -t MSAccess -d ./db/dbanalyze
	elif [[ $1 == 'sqlite' ]]; then
		./cli/bin/dbanalyze_cli -t SQLite -d ./db/dbanalyze.sqlite
	elif [[ $1 == 'graph' ]]; then
		./cli/bin/dbanalyze_cli -h localhost -p 5433 -u dbanalyze -d analyzetest -t PostgreSQL -w dbanalyze -graph
	elif [[ $1 == 'gui' ]]; then
		./gui/bin/dbanalyze_gui
	else
		./cli/bin/dbanalyze_cli -h localhost -p 5433 -u dbanalyze -d analyzetest -t PostgreSQL -w dbanalyze
	fi

	if [[ $lines -gt 1 ]]; then
		echo '========== Warnings =========';
		cat compilerlog
	fi
else
	echo '========== Compilation Failed =========';
	if [[ $lines -gt 20 ]]; then
		less compilerlog
	else
		cat compilerlog
	fi
fi
