#ifndef STDHEADERS_H
#define STDHEADERS_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define snprintf sprintf_s
#endif
#ifdef _MSC_VER
#ifndef __cplusplus
#define inline __inline
#endif
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
#include <QApplication>
#include <QMenu>
#endif

#endif // STDHEADERS_H

