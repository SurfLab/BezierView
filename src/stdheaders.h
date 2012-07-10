#ifndef STDHEADERS_H
#define STDHEADERS_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif


// Standard C++ libraries
#include <stdio.h>

// Graphics Libraries
#include <GL/glu.h>
#define GLUTLITE_STATIC 1
#include <GL/glutlite.h>


// Qt Libraries
#include <QtCore>
#include <QDebug>
#include <QDir>
#include <QFileDialog>

#endif // STDHEADERS_H

