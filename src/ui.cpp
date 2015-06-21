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




static const int MENU_MULTIPLIER = 1 << 20;



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
QAction* BViewUI::addMenuAction(QMenu* parent, QString title, int entry, int parameter, const char * shortcut, bool checkable, bool checked)
{
    int data = entry * MENU_MULTIPLIER + parameter;
    QAction* a = parent->addAction(title);
    a->setData(data); a->setCheckable(checkable); a->setChecked(checked);
    if(shortcut) a->setShortcut(QKeySequence(shortcut));
    QObject::connect(a, SIGNAL(triggered()), _signalMapper, SLOT(map()));
    _signalMapper->setMapping(a, data);
    return a;
}

void BViewUI::updateMenuAction(int entry, int parameter, bool checked, bool visible)
{
    int data = entry * MENU_MULTIPLIER + parameter;
    QAction *a = dynamic_cast<QAction*>(_signalMapper->mapping(data));
    a->setChecked(checked);
    a->setVisible(visible);
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

    addMenuAction(menuContext, "Zoom"  , MENUCONTROL_MOUSEMODE , MOUSEMODE_ZOOM  , NULL, true, g_mouseMode == MOUSEMODE_ZOOM  );
    addMenuAction(menuContext, "Rotate", MENUCONTROL_MOUSEMODE , MOUSEMODE_ROTATE, NULL, true, g_mouseMode == MOUSEMODE_ROTATE);
    addMenuAction(menuContext, "Move"  , MENUCONTROL_MOUSEMODE , MOUSEMODE_MOVE  , NULL, true, g_mouseMode == MOUSEMODE_MOVE  );

    addMenuAction(menuContext, "Clip Near", MENUCONTROL_MOUSEMODE , MOUSEMODE_CLIPNEAR, NULL, true, g_mouseMode == MOUSEMODE_CLIPNEAR);
    addMenuAction(menuContext, "Clip Far" , MENUCONTROL_MOUSEMODE , MOUSEMODE_CLIPFAR , NULL, true, g_mouseMode == MOUSEMODE_CLIPFAR );

    menuContext->addSeparator();

    addMenuAction(menuContext, "Clear Clipping", MENUCONTROL_CLIPSTOP);
    addMenuAction(menuContext, "Reset Projection", MENUCONTROL_RESET_PROJECTION);

    menuContext->addSeparator();

    _menuGroup = menuContext->addMenu("Groups");


    QMenu   *menuDisplay    = menuContext->addMenu("Display");
    addMenuAction(menuDisplay, "Patch", MENUCONTROL_DISPLAY , DRAWFLAGS_PATCH, "P", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_PATCH));
    addMenuAction(menuDisplay, "Control Mesh", MENUCONTROL_DISPLAY , DRAWFLAGS_MESH, "M", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_MESH));
    addMenuAction(menuDisplay, "Polygon Face", MENUCONTROL_DISPLAY , DRAWFLAGS_POLYPATCH, "P", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_POLYPATCH));
    addMenuAction(menuDisplay, "Polygon Mesh", MENUCONTROL_DISPLAY , DRAWFLAGS_POLYMESH, "M", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_POLYMESH));
    menuDisplay->addSeparator();
    addMenuAction(menuDisplay, "Curvature", MENUCONTROL_DISPLAY , DRAWFLAGS_CRV, "C", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_CRV));
    addMenuAction(menuDisplay, "Curvature Needles", MENUCONTROL_DISPLAY , DRAWFLAGS_CRVNEEDLE, "N", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_CRVNEEDLE));
    addMenuAction(menuDisplay, "Highlight Lines", MENUCONTROL_DISPLAY , DRAWFLAGS_HIGHLIGHT, "R", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_HIGHLIGHT));
    addMenuAction(menuDisplay, "Reflection Lines", MENUCONTROL_DISPLAY , DRAWFLAGS_REFLLINE, NULL, true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_REFLLINE));
    addMenuAction(menuDisplay, "Environment Mapping", MENUCONTROL_DISPLAY , DRAWFLAGS_ENVMAPPING, "E", true, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_ENVMAPPING));
    addMenuAction(menuDisplay, "Bounding Box", MENUCONTROL_DISPLAY , DRAWFLAGS_BOX, "B", true, isDisplayFlagEnabled(0, DRAWFLAGS_BOX));
    addMenuAction(menuDisplay, "Smooth Shading", MENUCONTROL_DISPLAY  , DRAWFLAGS_SMOOTH, "D", true, isDisplayFlagEnabled(g_current_grp,DRAWFLAGS_SMOOTH));
    addMenuAction(menuDisplay, "Hidden Line Removal", MENUCONTROL_DISPLAY  , DRAWFLAGS_HIDDENLINE, NULL, true, isDisplayFlagEnabled(g_current_grp,DRAWFLAGS_HIDDENLINE));

    QMenu   *menuPatchDetail= menuContext->addMenu("Patch Detail");
    for(int i = 1; i <= 6; i++)
        addMenuAction(menuPatchDetail,QString("%1 %2 %1").arg(QString::number(1<<i)).arg(QChar(215)),
                      MENUCONTROL_SUBST,i, NULL, true, g_substs[g_current_grp]==i);

    QMenu   *menuMaterial   = menuContext->addMenu("Material");
    for(int c=0;c<COLORNUM; c++)
        addMenuAction(menuMaterial, mat_name[c], MENUCONTROL_COLOR , c, NULL, true, g_Material[g_current_grp] == c);
    QAction *customColor = menuMaterial->addAction("Custom Color...");
    customColor->setShortcut(Qt::Key_F5);
    QObject::connect(customColor,SIGNAL(triggered()), this, SLOT(colorDialog()));

    QMenu   *menuLineColor  = menuContext->addMenu("Line Color");
    for(int i=0;i<10;i++)
        addMenuAction(menuLineColor,g_penColorNames[i],MENUCONTROL_BLACKPEN,i,NULL, true,g_PenColor[g_current_grp] == i);

    QMenu   *menuBackColor  = menuContext->addMenu("Background Color");
    for(int i=0; i < 10; i++)
        addMenuAction(menuBackColor,g_BackColorNames[i],MENUCONTROL_BLACKBACK,i,NULL,true,i == back_choice);

    QMenu   *menuAdvanced   = menuContext->addMenu("Advanced Options");

    addMenuAction(menuAdvanced, "Flip Normals", MENUCONTROL_FLIPNORMAL,0, "F");
    menuAdvanced->addSeparator();
    for(int i = 0;i < 3;i++)
        addMenuAction(menuAdvanced, "Light " + QString::number(i), MENUCONTROL_LIGHT,i, NULL, true, light_switch[i]);
    menuAdvanced->addSeparator();
    addMenuAction(menuAdvanced, "Anti Aliasing", MENUCONTROL_ANTIALIAS,0, NULL, true, g_AntiAlias);
    QMenu   *menuLineWidth = menuAdvanced->addMenu("Line Width");
    for(int i = 1; i <= 5; i++ )
        addMenuAction(menuLineWidth,QString::number(i),MENUCONTROL_LINEWIDTH,i,NULL,true,g_LineWidth[g_current_grp]==i);
    menuAdvanced->addSeparator();
    QMenu   *menuHighlightLineDensity = menuAdvanced->addMenu("Highlight Line Density");
    addMenuAction(menuHighlightLineDensity, "Increase", MENUCONTROL_INHLDENSE,0, "+");
    addMenuAction(menuHighlightLineDensity, "Decrease", MENUCONTROL_DEHLDENSE,0, "-");
    QMenu   *menuCurvatureStyle = menuAdvanced->addMenu("Curvature Style");

    addMenuAction(menuCurvatureStyle, "Curvature Lines", MENUCONTROL_CRVSTYLE , 0,NULL,true, crv_style == 0);
    addMenuAction(menuCurvatureStyle, "Color Shade", MENUCONTROL_CRVSTYLE , 1,NULL,true, crv_style == 1);
    addMenuAction(menuCurvatureStyle, "Gray Shade",  MENUCONTROL_CRVSTYLE , 2,NULL,true, crv_style == 2);

    QMenu   *menuCurvature  = menuContext->addMenu("Curvature Type");
    addMenuAction(menuCurvature, "Gaussian", MENUCONTROL_CURVATURE_TYPE , GAUSS_CRV, NULL, true, crv_choice == GAUSS_CRV);
    addMenuAction(menuCurvature, "Mean", MENUCONTROL_CURVATURE_TYPE , MEAN_CRV, NULL, true, crv_choice == MEAN_CRV);
    addMenuAction(menuCurvature, "Max", MENUCONTROL_CURVATURE_TYPE , MAX_CRV, NULL, true, crv_choice == MAX_CRV);
    addMenuAction(menuCurvature, "Min", MENUCONTROL_CURVATURE_TYPE , MIN_CRV, NULL, true, crv_choice == MIN_CRV);
    addMenuAction(menuCurvature, QString("%1 %3 Gauss + %2 %3 Mean%4").arg(curvature_ratio_a).arg(curvature_ratio_b).arg(QChar(215)).arg(QChar(178))
                  , MENUCONTROL_CURVATURE_TYPE , SPECIAL_CRV, NULL, true, crv_choice == SPECIAL_CRV);
    menuContext->addSeparator();
    QAction *actionOpen = menuContext->addAction("Open BV file...");
    actionOpen->setShortcut(Qt::Key_F3);
    QObject::connect(actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));

    QMenu   *menuSavePosition= menuContext->addMenu("Save position");
    for(int i = 0;i < 6; i++)
        addMenuAction(menuSavePosition,QString::number(i), MENUCONTROL_SAVE , i);
    QMenu   *menuLoadPosition= menuContext->addMenu("Load position");
    for(int i = 0;i < 6; i++)
        addMenuAction(menuLoadPosition,QString::number(i), MENUCONTROL_LOAD , i);
    addMenuAction(menuContext, "Quit", MENUCONTROL_QUIT, 0, "Q");

    _contextMenu = menuContext;
}


void BViewUI::updateContextMenu()
{
    if(group_num > 0) // if there are more than one group
    {
        _menuGroup->clear();
        addMenuAction(_menuGroup, "All Groups", MENUCONTROL_GROUP, 0, NULL, true, g_current_grp == 0);
        _menuGroup->addSeparator();
        for(int i=1;i<=group_num; i++)
            addMenuAction(_menuGroup, group[i].name, MENUCONTROL_GROUP , i,NULL, true, g_current_grp == i);
    }

    updateMenuAction(MENUCONTROL_MOUSEMODE , MOUSEMODE_ZOOM  , g_mouseMode == MOUSEMODE_ZOOM  );
    updateMenuAction(MENUCONTROL_MOUSEMODE , MOUSEMODE_ROTATE, g_mouseMode == MOUSEMODE_ROTATE);
    updateMenuAction(MENUCONTROL_MOUSEMODE , MOUSEMODE_MOVE  , g_mouseMode == MOUSEMODE_MOVE  );

    updateMenuAction(MENUCONTROL_MOUSEMODE , MOUSEMODE_CLIPNEAR, g_mouseMode == MOUSEMODE_CLIPNEAR);
    updateMenuAction(MENUCONTROL_MOUSEMODE , MOUSEMODE_CLIPFAR , g_mouseMode == MOUSEMODE_CLIPFAR );

    updateMenuAction(MENUCONTROL_DISPLAY , DRAWFLAGS_PATCH, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_PATCH), has_patch);
    updateMenuAction(MENUCONTROL_DISPLAY , DRAWFLAGS_MESH,  isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_MESH), has_patch);
    updateMenuAction(MENUCONTROL_DISPLAY , DRAWFLAGS_POLYPATCH, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_POLYPATCH), has_polygon);
    updateMenuAction(MENUCONTROL_DISPLAY , DRAWFLAGS_POLYMESH, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_POLYMESH), has_polygon);

    updateMenuAction(MENUCONTROL_DISPLAY , DRAWFLAGS_CRV, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_CRV));
    updateMenuAction(MENUCONTROL_DISPLAY , DRAWFLAGS_CRVNEEDLE, isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_CRVNEEDLE));
    updateMenuAction(MENUCONTROL_DISPLAY , DRAWFLAGS_HIGHLIGHT,isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_HIGHLIGHT));
    updateMenuAction(MENUCONTROL_DISPLAY , DRAWFLAGS_REFLLINE,  isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_REFLLINE));
    updateMenuAction(MENUCONTROL_DISPLAY , DRAWFLAGS_ENVMAPPING,  isDisplayFlagEnabled(g_current_grp, DRAWFLAGS_ENVMAPPING));
    updateMenuAction(MENUCONTROL_DISPLAY , DRAWFLAGS_BOX,  isDisplayFlagEnabled(0, DRAWFLAGS_BOX));
    updateMenuAction(MENUCONTROL_DISPLAY  , DRAWFLAGS_SMOOTH,  isDisplayFlagEnabled(g_current_grp,DRAWFLAGS_SMOOTH));
    updateMenuAction(MENUCONTROL_DISPLAY  , DRAWFLAGS_HIDDENLINE, isDisplayFlagEnabled(g_current_grp,DRAWFLAGS_HIDDENLINE));

    for(int i = 1; i <= 6; i++)
        updateMenuAction(MENUCONTROL_SUBST,i, g_substs[g_current_grp]==i);

    for(int c=0;c<COLORNUM; c++)
        updateMenuAction(MENUCONTROL_COLOR,c, g_Material[g_current_grp] == c);

    for(int i=0;i<10;i++)
        updateMenuAction(MENUCONTROL_BLACKPEN,i,g_PenColor[g_current_grp] == i);

    for(int i=0; i < 10; i++)
        updateMenuAction(MENUCONTROL_BLACKBACK,i, i == back_choice);


    for(int i = 0;i < 3;i++)
        updateMenuAction(MENUCONTROL_LIGHT,i, light_switch[i]);

    updateMenuAction( MENUCONTROL_ANTIALIAS,0,  g_AntiAlias);

    for(int i = 1; i <= 5; i++ )
        updateMenuAction(MENUCONTROL_LINEWIDTH,i, g_LineWidth[g_current_grp]==i);

    for(int i = 0; i < 3; i++)
        updateMenuAction(MENUCONTROL_CRVSTYLE,i, crv_style == i);

    updateMenuAction(MENUCONTROL_CURVATURE_TYPE , GAUSS_CRV, crv_choice == GAUSS_CRV);
    updateMenuAction(MENUCONTROL_CURVATURE_TYPE , MEAN_CRV,  crv_choice == MEAN_CRV);
    updateMenuAction(MENUCONTROL_CURVATURE_TYPE , MAX_CRV,  crv_choice == MAX_CRV);
    updateMenuAction(MENUCONTROL_CURVATURE_TYPE , MIN_CRV, crv_choice == MIN_CRV);
    updateMenuAction(MENUCONTROL_CURVATURE_TYPE , SPECIAL_CRV, crv_choice == SPECIAL_CRV, special_curv);
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
    menu_proc(entry / MENU_MULTIPLIER, entry % MENU_MULTIPLIER);
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
