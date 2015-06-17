#-------------------------------------------------
#
# Project created by QtCreator 2012-03-30T18:59:21
#
#-------------------------------------------------

QT       += core gui opengl

CONFIG += warn_off precompile_header
TARGET = bview
TEMPLATE = app

PRECOMPILED_DIR = "precompile"

gcc: QMAKE_CFLAGS += -std=c99

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG = glu

SOURCES += \
    src/curvature.c \
    src/util.c \
    src/highlight.c \
    src/light.c \
    src/menu.c \
    src/pick.c \
    src/rotate.c \
    src/texture.c \
    src/glob.c \
    src/draw.c \
    src/load.c \
    src/ui.cpp \
    src/export.c \
    src/patch.c \
    src/quadbezier.c \
    src/polygon.c \
    src/tribezier.c

HEADERS  += \
    src/texture.h \
    src/menu.h \
    src/glob.h \
    src/draw.h \
    src/util.h \
    src/type.h \
    src/highlight.h \
    src/curvature.h \
    src/light.h \
    src/pick.h \
    src/rotate.h \
    src/export.h \
    src/load.h \
    src/ui.h \
    src/patch.h \
    src/quadbezier.h \
    src/tribezier.h \
    src/polygon.h

INCLUDEPATH += $$PWD/src 

PRECOMPILED_HEADER = src/stdheaders.h


win32: LIBS += -L$$PWD/dependencies/lib/Win32/
win32: INCLUDEPATH += $$PWD/dependencies/include

RESOURCES += \
    src/bezierview.qrc

win32: RC_FILE = src/bezierview.rc

OTHER_FILES += \
    src/bezierview.rc
