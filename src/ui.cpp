#include "stdheaders.h"
extern "C" {
#include "type.h"
#include "patch.h"
#include "load.h"
#include "glob.h"
#include "menu.h"
#include "util.h"
#include "light.h"
#include "draw.h"
#include "rotate.h"
}
#include "ui.h"
#include "version.h"







void log_error(const char* context, const char* reason){
    QMessageBox e;
    e.setText(context);
    e.setInformativeText(reason);
    e.setStandardButtons(QMessageBox::Ok);
    e.setIcon(QMessageBox::Critical);
    e.exec();
}


/*!
 * \brief addMenuAction
 * Create a menu action with and attach it to the signal mapper. The data is the identifier that
 * is used for the mapping
 * \param parent
 * \param title
 * \param data
 * \param shortcut
 * \param checkable
 * \param checked
 * \return
 */
QAction* BViewUI::addMenuAction(QMenu* parent, QString title, int data, const char * shortcut, bool checkable, bool checked)
{
    QAction* a = parent->addAction(title);
    a->setData(data); a->setCheckable(checkable); a->setChecked(checked);
    if(shortcut) a->setShortcut(QKeySequence(shortcut));
    QObject::connect(a, SIGNAL(triggered()), _signalMapper, SLOT(map()));
    _signalMapper->setMapping(a, data);
    return a;
}

/*!
 * \brief createContextMenu
 * Create the default context menu for BezierView, the commands will be routed to parent's
 * command(int) slot.
 * \param parent
 * \return created menu
 */
void BViewUI::createContextMenu()
{
    /*
     * We need to set the icon later
    ui->actionOpen->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton, 0, this));
    ui->actionExit->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton, 0, this));
    */

    QMenu* menuContext = new QMenu(this);
    _signalMapper = new QSignalMapper(this);
    QObject::connect(_signalMapper, SIGNAL(mapped(int)), this, SLOT(command(int)));

    addMenuAction(menuContext, "Zoom"  , MENUCONTROL_ZOOM  , NULL, true, g_mouseMode == MENUCONTROL_ZOOM   && clip_item == 0);
    addMenuAction(menuContext, "Rotate", MENUCONTROL_ROTATE, NULL, true, g_mouseMode == MENUCONTROL_ROTATE && clip_item == 0);
    addMenuAction(menuContext, "Move"  , MENUCONTROL_MOVE  , NULL, true, g_mouseMode == MENUCONTROL_MOVE   && clip_item == 0);

    QMenu   *menuClipping   = menuContext->addMenu("Clipping");
    addMenuAction(menuClipping, "Clip Near", MENUCONTROL_CLIPNEAR, NULL, true, clip_item == MENUCONTROL_CLIPNEAR);
    addMenuAction(menuClipping, "Clip Far" , MENUCONTROL_CLIPFAR , NULL, true, clip_item == MENUCONTROL_CLIPFAR );
    addMenuAction(menuClipping, "Clear Clipping", MENUCONTROL_CLIPNEAR);

    menuContext->addSeparator();

    _menuGroup = menuContext->addMenu("Groups");


    QMenu   *menuDisplay    = menuContext->addMenu("Display");
    addMenuAction(menuDisplay, "Patch", MENUCONTROL_PATCH, "P", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_PATCH));
    addMenuAction(menuDisplay, "Control Mesh", MENUCONTROL_MESH, "M", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_MESH));
    addMenuAction(menuDisplay, "Polygon Face", MENUCONTROL_POLYPATCH, "P", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_POLYPATCH));
    addMenuAction(menuDisplay, "Polygon Mesh", MENUCONTROL_POLYMESH, "M", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_POLYMESH));
    menuDisplay->addSeparator();
    addMenuAction(menuDisplay, "Curvature", MENUCONTROL_CURVA, "C", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_CRV));
    addMenuAction(menuDisplay, "Curvature Needles", MENUCONTROL_CURVANEEDLE, "N", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_CRVNEEDLE));
    addMenuAction(menuDisplay, "Highlight Lines", MENUCONTROL_HIGHLIGHT, "R", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_HIGHLIGHT));
    addMenuAction(menuDisplay, "Reflection Lines", MENUCONTROL_REFLINE, NULL, true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_REFLLINE));
    addMenuAction(menuDisplay, "Environment Mapping", MENUCONTROL_ENVMAP, "E", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_ENVMAPPING));
    addMenuAction(menuDisplay, "Bounding Box", MENUCONTROL_DRAWBOX, "B", true, isDisplayFlagEnabled(0, DRAWFLAGS_BOX));

    QMenu   *menuPatchDetail= menuContext->addMenu("Patch Detail");
    for(int i = 1; i <= 6; i++)
        addMenuAction(menuPatchDetail,QString("%1 %2 %1").arg(QString::number(1<<i)).arg(QChar(215)),
                      MENUCONTROL_SUBST1-1+i, NULL, true, g_substs[g_current_grp]==i);

    QMenu   *menuMaterial   = menuContext->addMenu("Material");
    for(int c=0;c<COLORNUM; c++)
        addMenuAction(menuMaterial, mat_name[c], MENUCONTROL_COLOR0 + c, NULL, true, g_Material[g_current_grp] == c);
    QAction *customColor = menuMaterial->addAction("Custom Color...");
    customColor->setShortcut(Qt::Key_F5);
    QObject::connect(customColor,SIGNAL(triggered()), this, SLOT(colorDialog()));

    QMenu   *menuLineColor  = menuContext->addMenu("Line Color");
    for(int i=0;i<10;i++)
        addMenuAction(menuLineColor,g_penColorNames[i],MENUCONTROL_BLACKPEN+i,NULL, true,g_PenColor[g_current_grp] == i);

    QMenu   *menuBackColor  = menuContext->addMenu("Background Color");
    for(int i=0; i < 10; i++)
        addMenuAction(menuBackColor,g_BackColorNames[i],MENUCONTROL_BLACKBACK+i,NULL,true,i == back_choice);

    QMenu   *menuAdvanced   = menuContext->addMenu("Advanced Options");
    addMenuAction(menuAdvanced, "Smooth Shading", MENUCONTROL_SMOOTHSHD, "D", true, isDisplayFlagEnabled(g_current_grp,DRAWFLAGS_SMOOTH));
    addMenuAction(menuAdvanced, "Flip Normals", MENUCONTROL_FLIPNORMAL, "F");
    menuAdvanced->addSeparator();
    for(int i = 0;i < 3;i++)
        addMenuAction(menuAdvanced, "Light " + QString::number(i), MENUCONTROL_LIGHT0+i, NULL, true, light_switch[i]);
    menuAdvanced->addSeparator();
    addMenuAction(menuAdvanced, "Hidden Line Removal", MENUCONTROL_HDNLINERMV, NULL, true, isDisplayFlagEnabled(g_current_grp,DRAWFLAGS_HIDDENLINE));
    addMenuAction(menuAdvanced, "Anti Aliasing", MENUCONTROL_ANTIALIAS, NULL, true, g_AntiAlias);
    QMenu   *menuLineWidth = menuAdvanced->addMenu("Line Width");
    for(int i = 1; i <= 5; i++ )
        addMenuAction(menuLineWidth,QString::number(i),MENUCONTROL_LINEWIDTH1-1+i,NULL,true,g_LineWidth[g_current_grp]==i);
    menuAdvanced->addSeparator();
    QMenu   *menuHighlightLineDensity = menuAdvanced->addMenu("Highlight Line Density");
    addMenuAction(menuHighlightLineDensity, "Increase", MENUCONTROL_INHLDENSE, "+");
    addMenuAction(menuHighlightLineDensity, "Decrease", MENUCONTROL_DEHLDENSE, "-");
    QMenu   *menuCurvatureStyle = menuAdvanced->addMenu("Curvature Style");
    addMenuAction(menuCurvatureStyle, "Color Shade", MENUCONTROL_CRVSTYLE1,NULL,true, crv_style == 1);
    addMenuAction(menuCurvatureStyle, "Gray Shade", MENUCONTROL_CRVSTYLE2,NULL,true, crv_style == 2);
    addMenuAction(menuCurvatureStyle, "Curvature Lines", MENUCONTROL_CRVSTYLE0,NULL,true, crv_style == 0);

    QMenu   *menuCurvature  = menuContext->addMenu("Curvature Type");
    addMenuAction(menuCurvature, "Gaussian", MENUCONTROL_GAUSS_CRV, NULL, true, crv_choice == GAUSS_CRV);
    addMenuAction(menuCurvature, "Mean", MENUCONTROL_MEAN_CRV, NULL, true, crv_choice == MEAN_CRV);
    addMenuAction(menuCurvature, "Max", MENUCONTROL_MAX_CRV, NULL, true, crv_choice == MAX_CRV);
    addMenuAction(menuCurvature, "Min", MENUCONTROL_MIN_CRV, NULL, true, crv_choice == MIN_CRV);
    addMenuAction(menuCurvature, QString("%1 %3 Gauss + %2 %3 Mean%4").arg(curvature_ratio_a).arg(curvature_ratio_b).arg(QChar(215)).arg(QChar(178)), SPECIAL_CRV, NULL, true, crv_choice == SPECIAL_CRV);
    menuContext->addSeparator();
    QAction *actionOpen = menuContext->addAction("Open BV file...");
    actionOpen->setShortcut(Qt::Key_F3);
    QObject::connect(actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));

    QMenu   *menuSavePosition= menuContext->addMenu("Save position");
    for(int i = 0;i < 6; i++)
        addMenuAction(menuSavePosition,QString::number(i), MENUCONTROL_SAVE0 + i);
    QMenu   *menuLoadPosition= menuContext->addMenu("Load position");
    for(int i = 0;i < 6; i++)
        addMenuAction(menuLoadPosition,QString::number(i), MENUCONTROL_LOAD0 + i);
    addMenuAction(menuContext, "Quit", MENUCONTROL_QUIT, "Q");

    _contextMenu = menuContext;
}

void BViewUI::updateMenuAction(int id, bool checked, bool visible)
{
    QAction *a = dynamic_cast<QAction*>(_signalMapper->mapping(id));
    a->setChecked(checked);
    a->setVisible(visible);
}

void BViewUI::updateContextMenu()
{
    if(group_num > 0) // if there are more than one group
    {
        _menuGroup->clear();
        addMenuAction(_menuGroup, "All Groups", MENUCONTROL_ALLGROUPS, NULL, true, g_current_grp == 0);
        _menuGroup->addSeparator();
        for(int i=1;i<=group_num; i++)
            addMenuAction(_menuGroup, group[i].name, MENUCONTROL_GROUP1 + i - 1,NULL, true, g_current_grp == i);
    }

    updateMenuAction(MENUCONTROL_ZOOM  ,g_mouseMode == MENUCONTROL_ZOOM   && clip_item == 0);
    updateMenuAction(MENUCONTROL_ROTATE, g_mouseMode == MENUCONTROL_ROTATE && clip_item == 0);
    updateMenuAction( MENUCONTROL_MOVE , g_mouseMode == MENUCONTROL_MOVE   && clip_item == 0);

    updateMenuAction(MENUCONTROL_CLIPNEAR, clip_item == MENUCONTROL_CLIPNEAR);
    updateMenuAction(MENUCONTROL_CLIPFAR , clip_item == MENUCONTROL_CLIPFAR );

    updateMenuAction(MENUCONTROL_PATCH, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_PATCH), has_patch);
    updateMenuAction(MENUCONTROL_MESH,  isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_MESH), has_patch);
    updateMenuAction(MENUCONTROL_POLYPATCH, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_POLYPATCH), has_polygon);
    updateMenuAction(MENUCONTROL_POLYMESH, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_POLYMESH), has_polygon);

    updateMenuAction(MENUCONTROL_CURVA, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_CRV));
    updateMenuAction(MENUCONTROL_CURVANEEDLE, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_CRVNEEDLE));
    updateMenuAction(MENUCONTROL_HIGHLIGHT,isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_HIGHLIGHT));
    updateMenuAction(MENUCONTROL_REFLINE,  isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_REFLLINE));
    updateMenuAction(MENUCONTROL_ENVMAP,  isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_ENVMAPPING));

    for(int i = 1; i <= 6; i++)
        updateMenuAction(MENUCONTROL_SUBST1-1+i, g_substs[g_current_grp]==i);

    for(int c=0;c<COLORNUM; c++)
        updateMenuAction(MENUCONTROL_COLOR0+c, g_Material[g_current_grp] == c);

    for(int i=0;i<10;i++)
        updateMenuAction(MENUCONTROL_BLACKPEN+i,g_PenColor[g_current_grp] == i);

    for(int i=0; i < 10; i++)
        updateMenuAction(MENUCONTROL_BLACKBACK+i, i == back_choice);

    updateMenuAction(MENUCONTROL_SMOOTHSHD,  isDisplayFlagEnabled(g_current_grp,DRAWFLAGS_SMOOTH));

    for(int i = 0;i < 3;i++)
        updateMenuAction(MENUCONTROL_LIGHT0+i, light_switch[i]);

    updateMenuAction( MENUCONTROL_HDNLINERMV, isDisplayFlagEnabled(g_current_grp,DRAWFLAGS_HIDDENLINE));
    updateMenuAction( MENUCONTROL_ANTIALIAS,  g_AntiAlias);

    for(int i = 1; i <= 5; i++ )
        updateMenuAction(MENUCONTROL_LINEWIDTH1-1+i, g_LineWidth[g_current_grp]==i);

    updateMenuAction(MENUCONTROL_CRVSTYLE1, crv_style == 1);
    updateMenuAction(MENUCONTROL_CRVSTYLE2, crv_style == 2);
    updateMenuAction(MENUCONTROL_CRVSTYLE0, crv_style == 0);

    updateMenuAction(MENUCONTROL_GAUSS_CRV, crv_choice == GAUSS_CRV);
    updateMenuAction(MENUCONTROL_MEAN_CRV,  crv_choice == MEAN_CRV);
    updateMenuAction(MENUCONTROL_MAX_CRV,  crv_choice == MAX_CRV);
    updateMenuAction(MENUCONTROL_MIN_CRV, crv_choice == MIN_CRV);
    updateMenuAction(MENUCONTROL_SPECIAL_CRV, crv_choice == SPECIAL_CRV, special_curv);
}

BViewUI::BViewUI(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    _mainwindow = new QMainWindow;
    _mainwindow->setCentralWidget(this);

    setContextMenuPolicy(Qt::DefaultContextMenu);

    createContextMenu();

    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

void BViewUI::initializeGL(){
    initGL();


}

void BViewUI::contextMenuEvent(QContextMenuEvent *event)
{
    updateContextMenu();
    _contextMenu->exec(mapToGlobal(event->pos()));
}

static QGLWidget* currentGL = NULL;
void glDrawText(double x, double y, double z, const char * str)
{
    if(currentGL)
        currentGL->renderText(x, y, z, str);
}

void BViewUI::paintGL(){
    currentGL = this;
    display();
    currentGL = NULL;
}

QSize BViewUI::minimumSizeHint() const{
    return QSize(winWidth,winHeight);

}
QSize  BViewUI::sizeHint() const{
    return QSize(winWidth,winHeight);
}

void BViewUI::resizeGL(int width, int height){
    winWidth = width, winHeight = height;
    glViewport(0,0,winWidth,winHeight);
    updateProjection();
}

KeyboardModifier qt2key(Qt::KeyboardModifiers k)
{
    return (KeyboardModifier) (int) k;
}

void BViewUI::mousePressEvent(QMouseEvent *event){
    mouseButton(event->button(), event->buttons(), event->x(), event->y(), qt2key(event->modifiers()));
    update();
    QGLWidget::mousePressEvent(event);
}
void BViewUI::mouseReleaseEvent(QMouseEvent *event){
    mouseButton(event->button(), 2, event->x(), event->y(),qt2key(event->modifiers()));
    QGLWidget::mouseReleaseEvent(event);
}

void BViewUI::mouseMoveEvent(QMouseEvent *event){
    mouseMotion(event->x(), event->y(),qt2key(event->modifiers()));
    updateGL();
}

void BViewUI::wheelEvent(QWheelEvent *w){
    zoom( w->delta() );
    updateGL();
}

void BViewUI::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    switch(key) {
    case Qt::Key_Up: // up
        ViewCenter[1] -= 0.1*ViewSize;
        break;
    case Qt::Key_Left: // left
        ViewCenter[0] += 0.1*ViewSize;
        break;
    case Qt::Key_Down:  // down
        ViewCenter[1] += 0.1*ViewSize;
        break;
    case Qt::Key_Right:  // right
        ViewCenter[0] -= 0.1*ViewSize;
        break;
    case Qt::Key_F3:
        openFile();
        break;
    case Qt::Key_F5:
        colorDialog();
        break;
    default:
        keyboard(event->text()[0].toLatin1());
    }
    updateProjection();
    updateGL();
}

void BViewUI::colorDialog()
{
    QColor color;
    if (true)
        color = QColorDialog::getColor(Qt::green, this);
    else
        color = QColorDialog::getColor(Qt::green, this, "Select Color", QColorDialog::DontUseNativeDialog);

    if (color.isValid()) {
        color_proc_rgb(mkcolor3(color.red()/255.0f,color.green()/255.0f,color.blue()/255.0f));
        updateGL();
    }
}


void BViewUI::command(int entry)
{
    menu_proc(entry);
    updateGL();
}



void BViewUI::tryLoadFile(QString fn){
    if(!fn.isEmpty()){
        if(QFile::exists(fn)){
            _mainwindow->setWindowFilePath(fn);
            loadDataFile(fn.toLatin1().data());
            updateGL();
        }else{
            QMessageBox msg;
            msg.setText(fn);
            msg.setInformativeText("File Not Found");
            msg.setIcon(QMessageBox::Critical);
            msg.exec();
            _mainwindow->setWindowFilePath("(Unloaded)");
        }
    }else
        _mainwindow->setWindowFilePath("(Unloaded)");

}

const char* filter = "BezierView Files(*.bv);;Text Files(*.txt);;All Files(*.*)";

void BViewUI::saveFile()
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
QMainWindow *BViewUI::mainwindow() const
{
    return _mainwindow;
}



void BViewUI::openFile()
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




int main(int argc, char *argv[])
{
    printf("BezierView version %d.%d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_BUILD);
    QApplication a(argc, argv);
    init_bezierview(argc,argv);

    a.setApplicationName(QString("SurfLab BezierView %1.%2.%3.%4").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_PATCH).arg(VERSION_BUILD));
    BViewUI viewer;
    viewer.tryLoadFile(QString(dataFileName));

    viewer.mainwindow()->setWindowIcon(QIcon(":/bezierview.ico"));
    viewer.mainwindow()->show();
    return a.exec();
}
