TARGET = bin/dbanalyze_gui

SOURCES += $$PWD/src/*.cc

OBJECTS_DIR = build/

INCLUDEPATH += include/
INCLUDEPATH += ../lib/include/

QMAKE_CXXFLAGS += -std=gnu++11
CONFIG += link_pkgconfig

PKGCONFIG += gtkmm-3.0
#PKGCONFIG += libgda-5.0
PKGCONFIG += clutter-1.0
PKGCONFIG += clutter-gtk-1.0

CXXFLAGS += -Wall
CXX = g++

unix {
 	LIBS += -L$$PWD/../lib/bin/ -ldbanalyze
}

INCLUDEPATH += $$PWD/../lib/bin
DEPENDPATH += $$PWD/../lib/bin
