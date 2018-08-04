#-------------------------------------------------
#
# Project created by QtCreator 2015-08-27T19:58:57
#
#-------------------------------------------------

QT += sql xml core widgets testlib network
QT -= gui

CONFIG+=testcase
QMAKE_CXXFLAGS += -std=c++14

DEPENDPATH += $$PWD/../atools/src
INCLUDEPATH += $$PWD/../atools/src $$PWD/src

CONFIG(debug, debug|release):CONF_TYPE=debug
CONFIG(release, debug|release):CONF_TYPE=release

unix {
  LIBS += -L$$PWD/../build-atools-$${CONF_TYPE} -latools
  PRE_TARGETDEPS += $$PWD/../build-atools-$${CONF_TYPE}/libatools.a
}

unix:!macx {
  copydata.commands = cp -avfu $$PWD/testdata $$OUT_PWD
}

TARGET = atoolstest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    src/atoolstest.cpp \
    src/metartest.cpp \
    src/geotest.cpp \
    src/dtmtest.cpp \
    src/flightplantest.cpp \
    src/scenerycfgtest.cpp \
    src/magdectest.cpp \
    src/updatetest.cpp \
    src/stringtest.cpp \
    src/maintest.cpp \
    src/versiontest.cpp \
    src/onlinetest.cpp

HEADERS += \
    src/metartest.h \
    src/geotest.h \
    src/dtmtest.h \
    src/flightplantest.h \
    src/scenerycfgtest.h \
    src/magdectest.h \
    src/updatetest.h \
    src/stringtest.h \
    src/maintest.h \
    src/versiontest.h \
    src/onlinetest.h

DEFINES += SRCDIR=\\\"$$PWD/\\\"

DISTFILES += \
    uncrustify.cfg \
    README.txt \
    LICENSE.txt

RESOURCES += \
    resources.qrc


first.depends = $(first) copydata
QMAKE_EXTRA_TARGETS += first copydata
