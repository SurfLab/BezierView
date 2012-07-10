#-------------------------------------------------
#
# Project created by QtCreator 2012-03-30T18:59:21
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = BezierView
TEMPLATE = app

CONFIG += link_pkgconfig
PKGCONFIG = glutlite glu

SOURCES += \
    src/texture.cpp \
    src/rotate.cpp \
    src/pick.cpp \
    src/Patch.cpp \
    src/menu.cpp \
    src/mainwindow.cpp \
    src/main.cpp \
    src/light.cpp \
    src/glob.cpp \
    src/draw.cpp \
    src/bview.cpp \
    src/bvglwidget.cpp \
    src/SGOL/util.cpp \
    src/SGOL/TriBezier.cpp \
    src/SGOL/QuadBezier.cpp \
    src/SGOL/Polygon.cpp \
    src/SGOL/highlight.cpp \
    src/SGOL/curvature.cpp

HEADERS  += \
    src/texture.h \
    src/stdheaders.h \
    src/Patch.h \
    src/menu.h \
    src/mainwindow.h \
    src/glob.h \
    src/draw.h \
    src/bview.h \
    src/bvglwidget.h \
    src/SGOL/util.h \
    src/SGOL/type.h \
    src/SGOL/TriBezier.h \
    src/SGOL/QuadBezier.h \
    src/SGOL/Polygon.h \
    src/SGOL/Object.h \
    src/SGOL/highlight.h \
    src/SGOL/curvature.h \
    src/SGOL/Bezier.h

FORMS    += \
    src/mainwindow.ui

INCLUDEPATH += $$PWD/src 


win32: LIBS += -L$$PWD/dependencies/lib/Win32/
win32: INCLUDEPATH += $$PWD/dependencies/include

