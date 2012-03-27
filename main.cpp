#include <QtGui/QApplication>
#include "mainwindow.h"

int qtmain(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
