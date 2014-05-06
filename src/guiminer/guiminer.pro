#-------------------------------------------------
#
# Project created by QtCreator 2014-05-06T14:37:23
#
#-------------------------------------------------

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -rdynamic
QMAKE_CXXFLAGS += -DGUI
QMAKE_CFLAGS += -std=c11
QMAKE_CFLAGS += -Dstatic_assert=_Static_assert
QMAKE_CFLAGS += -D_GNU_SOURCE
INCLUDEPATH += ../../contrib/epee/include
INCLUDEPATH += ../
LIBS += -lpthread
LIBS += -lrt
LIBS += -lboost_program_options
LIBS += -lboost_system
LIBS += -lboost_thread
LIBS += -lboost_regex
LIBS += -lboost_date_time
LIBS += -lboost_chrono
LIBS += -lboost_filesystem
LIBS += -lboost_serialization
LIBS += -lboost_atomic
LIBS += ../../build/release/src/libcrypto.a
LIBS += ../../build/release/src/libcommon.a
LIBS += ../../build/release/src/libwallet.a
LIBS += ../../build/release/src/librpc.a
LIBS += ../../build/release/src/libcryptonote_core.a

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = guiminer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../minerc/minerc.cpp \
    ../common/base58.cpp \
    ../crypto/slow-hash.c \
    ../cryptonote_core/miner.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
