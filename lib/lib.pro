TARGET = bin/dbanalyze

SOURCES += $$PWD/src/*.cc

OBJECTS_DIR = build/

INCLUDEPATH += include/

CONFIG += link_pkgconfig

TEMPLATE = lib
PKGCONFIG += libgda-5.0

QMAKE_CXXFLAGS += -std=gnu++11
CXXFLAGS = -Wall
CXXFLAGS += -fPIC

CXX = g++

LIBS += -lcgraph
LIBS += -lgvc
