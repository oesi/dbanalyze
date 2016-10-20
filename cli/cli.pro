TARGET = bin/dbanalyze_cli

SOURCES += $$PWD/src/*.cc

OBJECTS_DIR = build/

INCLUDEPATH += include/
INCLUDEPATH += ../lib/include/

QMAKE_CXXFLAGS += -std=gnu++11
CONFIG += link_pkgconfig

PKGCONFIG += libgda-5.0

CXXFLAGS += -Wall
CXX = g++

unix {
 	LIBS += -L$$PWD/../lib/bin/ -ldbanalyze
	LIBS += -lboost_program_options
}

INCLUDEPATH += $$PWD/../lib/bin
DEPENDPATH += $$PWD/../lib/bin
