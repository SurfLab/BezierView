#include <QtGui/QApplication>
#include "mainwindow.h"
#include "bview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    init_bezierview(argc,argv);

    a.setApplicationName("SurfLab BezierView");
    MainWindow w;
    w.show();

    return a.exec();
}
