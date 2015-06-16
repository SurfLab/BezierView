#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menu.h"
#include "bview.h"

extern  char   mat_name[][20];

MainWindow::MainWindow(QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow)
{
   ui->setupUi(this);
   initializeViewMenu();
   ui->actionOpen->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton, 0, this));
   ui->actionExit->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton, 0, this));
   ui->centralWidget->setContextMenu(ui->menuContext);
   menuBar()->hide();
   tryLoadFile(QString(dataFileName));

   // Initialize sub menus
   for(int c=0;c<COLORNUM; c++)
   {
       QAction *a = ui->menuMaterial->addAction(mat_name[c]);
       a->setCheckable(true);
       a->setChecked(g_Material[g_current_grp] == c);
       a->setData(c);
   }
   for(int i=0;i<10;i++)
   {
       QAction *a = ui->menuLine_Color->addAction(g_penColorNames[i]);
       a->setCheckable(true);
       a->setChecked(g_PenColor[g_current_grp] == i);
       a->setData(BLACKPEN+i);
   }
   for(int i=0; i < 10; i++)
   {
       QAction *a = ui->menuBackground->addAction(g_BackColorNames[i]);
       a->setCheckable(true);
       a->setChecked(i == back_choice);
       a->setData(BLACKBACK+i);
   }
   for(int i = 0;i < 6; i++)
   {
       QAction *a = ui->menuSave_position->addAction(QString::number(i));
       a->setData(SAVE0 + i);
   }
   for(int i = 0;i < 6; i++)
   {
       QAction *a = ui->menuLoad_position->addAction(QString::number(i));
       a->setData(LOAD0 + i);
   }
   for(int i = 1; i <= 5; i++ )
   {
       QAction *a = ui->menuLine_Width->addAction(QString::number(i));
       a->setData(LINEWIDTH1-1+i);
       a->setCheckable(true);
       a->setChecked(g_LineWidth[g_current_grp]==i);
   }
   for(int i = 1; i <= 6; i++)
   {
       QAction *a = ui->menuPatch_Detail->addAction(QString::number(1<<i) + QString(QChar(10799)) + QString::number(1<<i));
       a->setData(SUBST1-1+i);
       a->setCheckable(true);
       a->setChecked(g_substs[g_current_grp]==i);
   }

}

MainWindow::~MainWindow()
{
   delete ui;
}

void MainWindow::tryLoadFile(QString fn){
    if(!fn.isEmpty()){
        if(QFile::exists(fn)){
            setWindowFilePath(fn);
            loadDataFile(fn.toLatin1().data());
            ui->centralWidget->updateGL();
        }else{
            QMessageBox msg;
            msg.setText(fn);
            msg.setInformativeText("File Not Found");
            msg.setIcon(QMessageBox::Critical);
            msg.exec();
            setWindowFilePath("(Unloaded)");
        }
    }else
        setWindowFilePath("(Unloaded)");

}

const char* filter = "BezierView Files(*.bv);;Text Files(*.txt);;All Files(*.*)";

void MainWindow::saveFile()
{
   QString fileName = QFileDialog::getSaveFileName(this,
                               tr("Save BezierView File"),
                                                   QString(),
                               tr("BezierView Files(*.bv);;Text Files(*.txt);;All Files(*.*)"),
                               0,
                               0);
   if (!fileName.isEmpty())
       qDebug()<< "Saving is not implemented yet " <<  fileName;
}


void MainWindow::setTitle(QString fn){

}

void MainWindow::openFile()
{
   QString fileName = QFileDialog::getOpenFileName(this,
                               tr("Open BezierView File"),
                                                   QString(),
                               tr("BezierView Files(*.bv);;Text Files(*.txt);;All Files(*.*)"),
                               0,
                               0);
   if (!fileName.isEmpty()){
       tryLoadFile(fileName);
   }


}
void MainWindow::initializeViewMenu(){
    QAction *normalAct = new QAction(tr("Normal"), this);
    normalAct->setCheckable(true);
    connect(normalAct, SIGNAL(triggered()), centralWidget(), SLOT(toggleNormal()));

    QAction *curvatureAct = new QAction(tr("Curvature"), this);
    curvatureAct->setCheckable(true);
    connect(curvatureAct, SIGNAL(triggered()), centralWidget(), SLOT(toggleCurva()));

    QAction *highlightAct = new QAction(tr("Highlight Lines"), this);
    highlightAct->setCheckable(true);
    connect(highlightAct, SIGNAL(triggered()), centralWidget(), SLOT(toggleHighlight()));

    QAction *reflectionAct = new QAction(tr("Reflection Lines"), this);
    reflectionAct->setCheckable(true);
    connect(reflectionAct, SIGNAL(triggered()), centralWidget(), SLOT(toggleRefline()));

   QActionGroup *alignmentGroup = new QActionGroup(this);
   alignmentGroup->addAction(normalAct);
   alignmentGroup->addAction(curvatureAct);
   alignmentGroup->addAction(reflectionAct);
   alignmentGroup->addAction(highlightAct);
   normalAct->setChecked(true);
   QList<QMenu*> menus = menuBar()->findChildren<QMenu*>();
   QMenu *viewMenu = new QMenu(tr("View"), this);
   menus[1]->addMenu(viewMenu);
   viewMenu->addAction(normalAct);
   viewMenu->addAction(curvatureAct);
   viewMenu->addAction(highlightAct);
   viewMenu->addAction(reflectionAct);
}



