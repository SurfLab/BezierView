#include <QtGui/QApplication>
#include "mainwindow.h"
#include "bview.h"

int main(int argc, char *argv[])
{
    init_bezierview(argc,argv);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
