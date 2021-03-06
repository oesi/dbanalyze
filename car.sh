#!/bin/bash

if [[ $1 == 'clean' ]]; then
	make -C lib clean 1> /dev/null
	make -C gui clean 1> /dev/null
	make -C cli clean 1> /dev/null
	echo "Cleaned"
	exit
fi

# Export Lib Path
export LD_LIBRARY_PATH=.export LD_LIBRARY_PATH=./lib/bin

echo '' > compilerlog

#### Compile LIB ####
ret_lib=0
qmake lib/lib.pro -o lib/ && make -C lib 2>> compilerlog 1> /dev/null
ret_lib=$?

#### Compile CLI ####
ret_cli=0
qmake cli/cli.pro -o cli/ && make -C cli 2>> compilerlog 1> /dev/null
ret_cli=$?

#### Compile GUI ####
ret_gui=0
qmake gui/gui.pro -o gui/ && make -C gui 2>> compilerlog 1> /dev/null
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
	elif [[ $1 == 'full' ]]; then
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