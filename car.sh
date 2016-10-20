#!/bin/bash

# Export Lib Path
export LD_LIBRARY_PATH=.export LD_LIBRARY_PATH=./lib/bin

#### Compile LIB ####
qmake lib/lib.pro && make -C lib > /dev/null


#### Compile CLI ####
qmake cli/cli.pro && make -C cli > /dev/null


#### Compile GUI ####
qmake gui/gui.pro && make -C gui > /dev/null

lines="`cat compilerlog | wc -l`"
# Start program or display error messages
#./cli/bin/dbanalyze_cli -h localhost -p 5433 -u dbanalyze -d analyzetest -t PostgreSQL -w dbanalyze -xaa | java -jar /home/oesi/Downloads/plantuml.jar -pipe > diag.png
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
