#include "error.h"
#include <QtGui>


void error(const char* context, const char* reason){
    QMessageBox e;
    e.setText(context);
    e.setInformativeText(reason);
    e.setStandardButtons(QMessageBox::Ok);
    e.setIcon(QMessageBox::Critical);
    e.exec();
}
