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
apt-get install libgda-5.0-dev
# PostgreSQL Support
apt-get install libgda-5.0-postgres
# MySQL Support
apt-get install libgda-5.0-mysql
# MSAccess Support
# maybe apt-get install mdbtools-dev
# if libgda-5.0-mdb is not in the repo you can download it and copy
# it to /usr/lib/libgda-5.0/providers
apt-get install libgda-5.0-mdb

# Commandline tools
apt-get install libgda-5.0-bin
```
# Commandline List Supported Databases
```
gda-sql-5.0 -L
```

Start ./car.sh to compile an run the application

## Run Application
./cli/bin/dbanalyze_cli --help

source export.sh
./cli/bin/dbanalyze_cli -t MySQL -h localhost -d dbanalyze -u username -p 3306
./cli/bin/dbanalyze_cli -t PostgreSQL -h localhost -d dbanalyze -u username -p 5432
./cli/bin/dbanalyze_cli -t SQLite -d ./db/dbanalyze.sqlite

## Additional Links

GNOME Data Access
https://developer.gnome.org/libgda/stable/