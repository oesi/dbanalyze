# dbanalyze

Tool to analyze Databases

Supported Databases
   * PostgeSQL
   * SQLite
   * MySQL
   * MSAccess

## Features
   * Lists the number of Tables
   * Lists the number of Columns
   * Lists the number of Entries in each Table
   * Show Tables without Primary Keys
   * Show Tables without Entries
   * Lists the number of Unique Constraints
   * Lists the number of Foreign Keys
   * Datatype missmatch between FK->PK

## Planned

   * List of Indices
   * tables without references
   * Comments on Tables
   * column naming conventions
   * Support for MSSQL

## Install

```

apt-get install qt4-qmake
apt-get install gtkmm-3.0-1
apt-get install libgda-5.0-dev
apt-get install graphviz

# PostgreSQL Support
apt-get install libgda-5.0-postgres
# MySQL Support
apt-get install libgda-5.0-mysql
# MSAccess Support
# maybe apt-get install mdbtools-dev
# if libgda-5.0-mdb is not in the repo you can download it and copy
# it to /usr/lib/libgda-5.0/providers
apt-get install libgda-5.0-mdb

Start ./car.sh gui to compile an run the application

## Run Commandline Application
./cli/bin/dbanalyze_cli --help

source export.sh
./cli/bin/dbanalyze_cli -t MySQL -h localhost -d dbanalyze -u username -p 3306
./cli/bin/dbanalyze_cli -t PostgreSQL -h localhost -d dbanalyze -u username -p 5432
./cli/bin/dbanalyze_cli -t SQLite -d ./db/dbanalyze.sqlite

# Draw Graph
./cli/bin/dbanalyze_cli -t PostgreSQL -h localhost -d dbanalyze -u username -p 5432 -graph

## Run GUI
-/gui/bin/dbanalyze_gui

## Build Debian Package
dpkg-buildpackage -us -uc

## Additional Links

GNOME Data Access
https://developer.gnome.org/libgda/stable/

# Debugging
# Commandline tools
apt-get install libgda-5.0-bin
```
# Commandline List Supported Databases
```
gda-sql-5.0 -L
```
