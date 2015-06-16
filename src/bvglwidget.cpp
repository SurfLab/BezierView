#include "bvglwidget.h"
#include "bview.h"
#include "menu.h"
#include <QFileDialog>
#include "draw.h"

extern  char   mat_name[][20];
extern int clip_item;


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
QAction* BVGLWidget::addMenuAction(QMenu* parent, QString title, int data, const char * shortcut, bool checkable, bool checked)
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
void BVGLWidget::createContextMenu()
{
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

    QMenu   *menuPatchDetail= menuContext->addMenu("PatchDetail");
    for(int i = 1; i <= 6; i++)
        addMenuAction(menuPatchDetail,QString::number(1<<i) + QString(QChar(10799)) + QString::number(1<<i),
                      SUBST1-1+i, NULL, true, g_substs[g_current_grp]==i);

    QMenu   *menuMaterial   = menuContext->addMenu("Material");
    for(int c=0;c<COLORNUM; c++)
        addMenuAction(menuMaterial, mat_name[c], COLOR0 + c, NULL, true, g_Material[g_current_grp] == c);

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
    addMenuAction(menuCurvature, QString("%1 ⨯ Gauss + %2 ⨯ Mean²").arg(curvature_ratio_a).arg(curvature_ratio_b), SPECIAL_CRV, NULL, true, crv_choice == SPECIAL_CRV);
    menuContext->addSeparator();
    QMenu   *menuSavePosition= menuContext->addMenu("Save position");
    for(int i = 0;i < 6; i++)
        addMenuAction(menuSavePosition,QString::number(i), SAVE0 + i);
    QMenu   *menuLoadPosition= menuContext->addMenu("Load position");
    for(int i = 0;i < 6; i++)
        addMenuAction(menuLoadPosition,QString::number(i), LOAD0 + i);
    addMenuAction(menuContext, "Quit", QUIT, "Q");

    _contextMenu = menuContext;
}

void BVGLWidget::updateMenuAction(int id, bool checked, bool visible)
{
    QAction *a = dynamic_cast<QAction*>(_signalMapper->mapping(id));
    a->setChecked(checked);
    a->setVisible(visible);
}

void BVGLWidget::updateContextMenu()
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

BVGLWidget::BVGLWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    setContextMenuPolicy(Qt::DefaultContextMenu);

    createContextMenu();

}

void BVGLWidget::initializeGL(){
    initGL();


}

void BVGLWidget::contextMenuEvent(QContextMenuEvent *event)
{
    updateContextMenu();
    _contextMenu->exec(mapToGlobal(event->pos()));
}

void BVGLWidget::paintGL(){
    display();
}

QSize BVGLWidget::minimumSizeHint() const{
    return QSize(winWidth,winHeight);

}
QSize  BVGLWidget::sizeHint() const{
    return QSize(winWidth,winHeight);
}

void BVGLWidget::resizeGL(int width, int height){
    winWidth = width, winHeight = height;
    glViewport(0,0,winWidth,winHeight);
    updateProjection();
}
void BVGLWidget::mousePressEvent(QMouseEvent *event){
    mouseButton(event->button(), event->buttons(), event->x(), event->y(), event->modifiers());
    update();
    QGLWidget::mousePressEvent(event);
}
void BVGLWidget::mouseReleaseEvent(QMouseEvent *event){
    mouseButton(event->button(), 2, event->x(), event->y(), event->modifiers());
    QGLWidget::mouseReleaseEvent(event);
}

void BVGLWidget::mouseMoveEvent(QMouseEvent *event){
    mouseMotion(event->x(), event->y(), event->modifiers());
    int change = HIGHLIGHT;
    updateGL();
}

void BVGLWidget::wheelEvent(QWheelEvent *w){
    zoom( w->delta() );
    updateGL();
}

void BVGLWidget::keyPressEvent(QKeyEvent *event){

}

void BVGLWidget::changeView(int change){
    menu_proc(change);
   updateGL();

}

void BVGLWidget::colorDialog()
{
    QColor color;
    if (true)
        color = QColorDialog::getColor(Qt::green, this);
    else
        color = QColorDialog::getColor(Qt::green, this, "Select Color", QColorDialog::DontUseNativeDialog);

    if (color.isValid()) {
        float colorArr[] = {color.red()/255.0f,color.green()/255.0f,color.blue()/255.0f};

        changeColor(colorArr);
    }
}

void BVGLWidget::changeColor(float rgb[]){
    color_proc_rgb(rgb);
    updateGL();
}

void BVGLWidget::command(int entry)
{
    menu_proc(entry);
    updateGL();
}

void BVGLWidget::toggleHighlight(){
    changeView(HIGHLIGHT);
}
void BVGLWidget::toggleSmooth(){
    changeView(SMOOTH);
}
void BVGLWidget::toggleMesh(){
    changeView(MESH);
    changeView(POLYMESH);
}
void BVGLWidget::togglePatch(){
    changeView(PATCH);
    changeView(POLYPATCH);
}
void BVGLWidget::togglePolyMesh(){
    changeView(POLYMESH);
}
void BVGLWidget::togglePolyPatch(){
    changeView(POLYPATCH);
}
void BVGLWidget::toggleCurva(){
    changeView(CURVA);
}
void BVGLWidget::toggleCurvaNeedle(){
    changeView(CURVANEEDLE);
}
void BVGLWidget::toggleRefline(){
    changeView(REFLINE);
}

void BVGLWidget::toggleNormal(){
    changeView(NORMAL);
}
