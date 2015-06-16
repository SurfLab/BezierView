#ifndef STDHEADERS_H
#define STDHEADERS_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif


// Standard C++ libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 	/* sqrt */
#include <memory.h>
#include <time.h>  // gettimeofday
#include <stdbool.h>


// Graphics Libraries
#include <GL/glu.h>


#ifdef __cplusplus
// Qt Libraries
#include <string>
#include <QtCore>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QtOpenGL>
#include <QtGui/QApplication>
#include <QtGui/QMenu>
#endif

#endif // STDHEADERS_H

