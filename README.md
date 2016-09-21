# dbanalyze

Tool to analyze Databases

## Features
   * Lists the number of Tables
   * Lists the number of Columns
   * Lists the number of Entries in each Table
   * Show Tables without Primary Keys
   * Show Tables without Entries
   * Lists the number of Unique Constraints
   * Lists the number of Foreign Keys
   * Datatype missmatch between FK->PK

## Planned Checks

   * List of Indices
   * tables without references
   * Comments on Tables
   * column naming conventions

## Install

```
apt-get install libgda-5.0-dev
apt-get install libgda-5.0-postgres
```
Start ./car.sh to compile an run the application

## Run Application
./cli/bin/dbanalyze_cli --help

source export.sh
./cli/bin/dbanalyze_cli -t MySQL -d dbanalyze -u root -h localhost -p 3306
./cli/bin/dbanalyze_cli -p 5433 -h localhost -d devfhcomplete3 -u oesi -t PostgreSQL
./cli/bin/dbanalyze_cli -t sqlite -d ./db/dbanalyze.sqlite

## Additional Links

GNOME Data Access
https://developer.gnome.org/libgda/stable/