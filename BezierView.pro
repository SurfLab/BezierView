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

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG = glu

SOURCES += \
    src/texture.cpp \
    src/rotate.cpp \
    src/pick.cpp \
    src/Patch.cpp \
    src/menu.cpp \
    src/light.cpp \
    src/glob.cpp \
    src/draw.cpp \
    src/bview.cpp \
    src/bvglwidget.cpp \
    src/util.cpp \
    src/TriBezier.cpp \
    src/QuadBezier.cpp \
    src/Polygon.cpp \
    src/highlight.cpp \
    src/curvature.cpp \
    src/error.cpp

HEADERS  += \
    src/texture.h \
    src/Patch.h \
    src/menu.h \
    src/glob.h \
    src/draw.h \
    src/bview.h \
    src/bvglwidget.h \
    src/util.h \
    src/type.h \
    src/TriBezier.h \
    src/QuadBezier.h \
    src/Polygon.h \
    src/Object.h \
    src/highlight.h \
    src/curvature.h \
    src/Bezier.h \
    src/error.h

INCLUDEPATH += $$PWD/src 

PRECOMPILED_HEADER = src/stdheaders.h


win32: LIBS += -L$$PWD/dependencies/lib/Win32/
win32: INCLUDEPATH += $$PWD/dependencies/include

RESOURCES +=

win32: RC_FILE = src/bezierview.rc

OTHER_FILES += \
    src/bezierview.rc
