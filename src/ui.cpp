#include "stdheaders.h"
#include "ui.h"
extern "C" {
#include "type.h"
#include "Patch.h"
#include "load.h"
#include "glob.h"
#include "menu.h"
#include "util.h"
#include "light.h"
#include "draw.h"
#include "rotate.h"
}







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

    addMenuAction(menuContext, "Zoom"  , ZOOM  , NULL, true, g_mouseMode == ZOOM   && clip_item == 0);
    addMenuAction(menuContext, "Rotate", ROTATE, NULL, true, g_mouseMode == ROTATE && clip_item == 0);
    addMenuAction(menuContext, "Move"  , MOVE  , NULL, true, g_mouseMode == MOVE   && clip_item == 0);

    QMenu   *menuClipping   = menuContext->addMenu("Clipping");
    addMenuAction(menuClipping, "Clip Near", CLIPNEAR, NULL, true, clip_item == CLIPNEAR);
    addMenuAction(menuClipping, "Clip Far" , CLIPFAR , NULL, true, clip_item == CLIPFAR );
    addMenuAction(menuClipping, "Clear Clipping", CLIPNEAR);

    menuContext->addSeparator();

    if(group_num > 0) // if there are more than one group
    {
        QMenu* menuGroup = menuContext->addMenu("Groups");
        addMenuAction(menuGroup, "All Groups", ALLGROUPS, NULL, true, g_current_grp == 0);
        menuGroup->addSeparator();
        for(int i=1;i<=group_num; i++)
            addMenuAction(menuGroup, group[i].name, GROUP1 + i - 1,NULL, true, g_current_grp == i);
    }


    QMenu   *menuDisplay    = menuContext->addMenu("Display");
    addMenuAction(menuDisplay, "Patch", PATCH, "P", true, isDisplayFlagEnabled(g_current_grp, DRAWPATCH));
    addMenuAction(menuDisplay, "Control Mesh", MESH, "M", true, isDisplayFlagEnabled(g_current_grp, DRAWMESH));
    addMenuAction(menuDisplay, "Polygon Face", POLYPATCH, "P", true, isDisplayFlagEnabled(g_current_grp, DRAWPOLYPATCH));
    addMenuAction(menuDisplay, "Polygon Mesh", POLYMESH, "M", true, isDisplayFlagEnabled(g_current_grp, DRAWPOLYMESH));
    menuDisplay->addSeparator();
    addMenuAction(menuDisplay, "Curvature", CURVA, "C", true, isDisplayFlagEnabled(g_current_grp, DRAWCRV));
    addMenuAction(menuDisplay, "Curvature Needles", CURVANEEDLE, "N", true, isDisplayFlagEnabled(g_current_grp, DRAWCRVNEEDLE));
    addMenuAction(menuDisplay, "Highlight Lines", HIGHLIGHT, "R", true, isDisplayFlagEnabled(g_current_grp, DRAWHIGHLIGHT));
    addMenuAction(menuDisplay, "Reflection Lines", REFLINE, NULL, true, isDisplayFlagEnabled(g_current_grp, DRAWREFLLINE));
    addMenuAction(menuDisplay, "Environment Mapping", ENVMAP, "E", true, isDisplayFlagEnabled(g_current_grp, ENVMAPPING));
    addMenuAction(menuDisplay, "Bounding Box", DRAWBOX, "B");

    QMenu   *menuPatchDetail= menuContext->addMenu("Patch Detail");
    for(int i = 1; i <= 6; i++)
        addMenuAction(menuPatchDetail,QString("%1 %2 %1").arg(QString::number(1<<i)).arg(QChar(215)),
                      SUBST1-1+i, NULL, true, g_substs[g_current_grp]==i);

    QMenu   *menuMaterial   = menuContext->addMenu("Material");
    for(int c=0;c<COLORNUM; c++)
        addMenuAction(menuMaterial, mat_name[c], COLOR0 + c, NULL, true, g_Material[g_current_grp] == c);
    QAction *customColor = menuMaterial->addAction("Custom Color...");
    customColor->setShortcut(Qt::Key_F5);
    QObject::connect(customColor,SIGNAL(triggered()), this, SLOT(colorDialog()));

    QMenu   *menuLineColor  = menuContext->addMenu("Line Color");
    for(int i=0;i<10;i++)
        addMenuAction(menuLineColor,g_penColorNames[i],BLACKPEN+i,NULL, true,g_PenColor[g_current_grp] == i);

    QMenu   *menuBackColor  = menuContext->addMenu("Background Color");
    for(int i=0; i < 10; i++)
        addMenuAction(menuBackColor,g_BackColorNames[i],BLACKBACK+i,NULL,true,i == back_choice);

    QMenu   *menuAdvanced   = menuContext->addMenu("Advanced Options");
    addMenuAction(menuAdvanced, "Smooth Shading", SMOOTHSHD, "D", true, isDisplayFlagEnabled(g_current_grp,SMOOTH));
    addMenuAction(menuAdvanced, "Flip Normals", FLIPNORMAL, "F");
    menuAdvanced->addSeparator();
    for(int i = 0;i < 3;i++)
        addMenuAction(menuAdvanced, "Light " + QString::number(i), MENU_LIGHT0+i, NULL, true, light_switch[i]);
    menuAdvanced->addSeparator();
    addMenuAction(menuAdvanced, "Hidden Line Removal", HDNLINERMV, NULL, true, isDisplayFlagEnabled(g_current_grp,HIDDENLINE));
    addMenuAction(menuAdvanced, "Anti Aliasing", ANTIALIAS, NULL, true, g_AntiAlias);
    QMenu   *menuLineWidth = menuAdvanced->addMenu("Line Width");
    for(int i = 1; i <= 5; i++ )
        addMenuAction(menuLineWidth,QString::number(i),LINEWIDTH1-1+i,NULL,true,g_LineWidth[g_current_grp]==i);
    menuAdvanced->addSeparator();
    QMenu   *menuHighlightLineDensity = menuAdvanced->addMenu("Highlight Line Density");
    addMenuAction(menuHighlightLineDensity, "Increase", INHLDENSE, "+");
    addMenuAction(menuHighlightLineDensity, "Decrease", DEHLDENSE, "-");
    QMenu   *menuCurvatureStyle = menuAdvanced->addMenu("Curvature Style");
    addMenuAction(menuCurvatureStyle, "Color Shade", CRVSTYLE1,NULL,true, crv_style == 1);
    addMenuAction(menuCurvatureStyle, "Gray Shade", CRVSTYLE2,NULL,true, crv_style == 2);
    addMenuAction(menuCurvatureStyle, "Curvature Lines", CRVSTYLE0,NULL,true, crv_style == 0);

    QMenu   *menuCurvature  = menuContext->addMenu("Curvature Type");
    addMenuAction(menuCurvature, "Gaussian", GAUSS_CRV, NULL, true, crv_choice == GAUSS_CRV);
    addMenuAction(menuCurvature, "Mean", MEAN_CRV, NULL, true, crv_choice == MEAN_CRV);
    addMenuAction(menuCurvature, "Max", MAX_CRV, NULL, true, crv_choice == MAX_CRV);
    addMenuAction(menuCurvature, "Min", MIN_CRV, NULL, true, crv_choice == MIN_CRV);
    addMenuAction(menuCurvature, QString("%1 %3 Gauss + %2 %3 Mean%4").arg(curvature_ratio_a).arg(curvature_ratio_b).arg(QChar(215)).arg(QChar(178)), SPECIAL_CRV, NULL, true, crv_choice == SPECIAL_CRV);
    menuContext->addSeparator();
    QAction *actionOpen = menuContext->addAction("Open BV file...");
    actionOpen->setShortcut(Qt::Key_F3);
    QObject::connect(actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));

    QMenu   *menuSavePosition= menuContext->addMenu("Save position");
    for(int i = 0;i < 6; i++)
        addMenuAction(menuSavePosition,QString::number(i), SAVE0 + i);
    QMenu   *menuLoadPosition= menuContext->addMenu("Load position");
    for(int i = 0;i < 6; i++)
        addMenuAction(menuLoadPosition,QString::number(i), LOAD0 + i);
    addMenuAction(menuContext, "Quit", QUIT, "Q");

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
        updateMenuAction(ALLGROUPS, g_current_grp == 0);
        for(int i=1;i<=group_num; i++)
            updateMenuAction(GROUP1 + i - 1, g_current_grp == i);
    }

    updateMenuAction(ZOOM  ,g_mouseMode == ZOOM   && clip_item == 0);
    updateMenuAction(ROTATE, g_mouseMode == ROTATE && clip_item == 0);
    updateMenuAction( MOVE , g_mouseMode == MOVE   && clip_item == 0);

    updateMenuAction(CLIPNEAR, clip_item == CLIPNEAR);
    updateMenuAction(CLIPFAR , clip_item == CLIPFAR );

    updateMenuAction(PATCH, isDisplayFlagEnabled(g_current_grp, DRAWPATCH), has_patch);
    updateMenuAction(MESH,  isDisplayFlagEnabled(g_current_grp, DRAWMESH), has_patch);
    updateMenuAction(POLYPATCH, isDisplayFlagEnabled(g_current_grp, DRAWPOLYPATCH), has_polygon);
    updateMenuAction(POLYMESH, isDisplayFlagEnabled(g_current_grp, DRAWPOLYMESH), has_polygon);

    updateMenuAction(CURVA, isDisplayFlagEnabled(g_current_grp, DRAWCRV));
    updateMenuAction(CURVANEEDLE, isDisplayFlagEnabled(g_current_grp, DRAWCRVNEEDLE));
    updateMenuAction(HIGHLIGHT,isDisplayFlagEnabled(g_current_grp, DRAWHIGHLIGHT));
    updateMenuAction(REFLINE,  isDisplayFlagEnabled(g_current_grp, DRAWREFLLINE));
    updateMenuAction(ENVMAP,  isDisplayFlagEnabled(g_current_grp, ENVMAPPING));

    for(int i = 1; i <= 6; i++)
        updateMenuAction(SUBST1-1+i, g_substs[g_current_grp]==i);

    for(int c=0;c<COLORNUM; c++)
        updateMenuAction(COLOR0+c, g_Material[g_current_grp] == c);

    for(int i=0;i<10;i++)
        updateMenuAction(BLACKPEN+i,g_PenColor[g_current_grp] == i);

    for(int i=0; i < 10; i++)
        updateMenuAction(BLACKBACK+i, i == back_choice);

    updateMenuAction(SMOOTHSHD,  isDisplayFlagEnabled(g_current_grp,SMOOTH));

    for(int i = 0;i < 3;i++)
        updateMenuAction(MENU_LIGHT0+i, light_switch[i]);

    updateMenuAction( HDNLINERMV, isDisplayFlagEnabled(g_current_grp,HIDDENLINE));
    updateMenuAction( ANTIALIAS,  g_AntiAlias);

    for(int i = 1; i <= 5; i++ )
        updateMenuAction(LINEWIDTH1-1+i, g_LineWidth[g_current_grp]==i);

    updateMenuAction(CRVSTYLE1, crv_style == 1);
    updateMenuAction(CRVSTYLE2, crv_style == 2);
    updateMenuAction(CRVSTYLE0, crv_style == 0);

    updateMenuAction(GAUSS_CRV, crv_choice == GAUSS_CRV);
    updateMenuAction(MEAN_CRV,  crv_choice == MEAN_CRV);
    updateMenuAction(MAX_CRV,  crv_choice == MAX_CRV);
    updateMenuAction(MIN_CRV, crv_choice == MIN_CRV);
    updateMenuAction(SPECIAL_CRV, crv_choice == SPECIAL_CRV, special_curv);
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
void glDrawText(float x, float y, float z, const char * str)
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
        float rgb[] = {color.red()/255.0f,color.green()/255.0f,color.blue()/255.0f};

        color_proc_rgb(rgb);
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
    QApplication a(argc, argv);
    init_bezierview(argc,argv);

    a.setApplicationName("SurfLab BezierView");
    BViewUI viewer;
    viewer.tryLoadFile(QString(dataFileName));

    viewer.mainwindow()->setWindowIcon(QIcon(":/bezierview.ico"));
    viewer.mainwindow()->show();
    return a.exec();
}
