TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CFLAGS += -I../common

SOURCES += ../common/md5.c ../common/cmp.c ../common/iarch_mgmt.c
SOURCES += ../common/iarch_stor.c ../common/fixbuffer.c ../common/fixarray.c

SOURCES += cli-trans.c cli-mgmt.c cli-stor.c session.c main.c
