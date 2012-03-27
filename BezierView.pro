#-------------------------------------------------
#
# Project created by QtCreator 2012-03-27T10:59:53
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = BezierView
TEMPLATE= app


SOURCES += main.cpp\
        mainwindow.cpp \
    texture.cpp \
    rotate.cpp \
    pick.cpp \
    Patch.cpp \
    main.cpp \
    light.cpp \
    glob.cpp \
    draw.cpp \
    bview.cpp \
    SGOL/util.cpp \
    SGOL/TriBezier.cpp \
    SGOL/QuadBezier.cpp \
    SGOL/Polygon.cpp \
    SGOL/highlight.cpp \
    SGOL/curvature.cpp \
    bvglut.cpp \
    menu.cpp \
    bvglwidget.cpp

HEADERS  += mainwindow.h \
    texture.h \
    Patch.h \
    glob.h \
    draw.h \
    SGOL/util.h \
    SGOL/type.h \
    SGOL/TriBezier.h \
    SGOL/QuadBezier.h \
    SGOL/Polygon.h \
    SGOL/Object.h \
    SGOL/highlight.h \
    SGOL/curvature.h \
    SGOL/Bezier.h \
    bview.h \
    stdheaders.h \
    menu.h \
    bvglwidget.h

FORMS    += mainwindow.ui


win32: LIBS += -L$$PWD/dependencies/lib/Win32/ -lfreeglut_static

INCLUDEPATH += SGOL $$PWD/dependencies/include
DEPENDPATH += $$PWD/dependencies/include

win32: PRE_TARGETDEPS += $$PWD/dependencies/lib/Win32/freeglut_static.lib
