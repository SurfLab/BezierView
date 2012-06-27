#include "bvglwidget.h"
#include "bview.h"
#include "menu.h"
#include <QFileDialog>

BVGLWidget::BVGLWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{

}

void BVGLWidget::initializeGL(){
    initGL();

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

void BVGLWidget::resizeGL(int width, int height){}
void BVGLWidget::mousePressEvent(QMouseEvent *event){
    mouseButton(event->button(), event->buttons(), event->x(), event->y(), event->modifiers());
    update();
    //draw();
    }
void BVGLWidget::mouseReleaseEvent(QMouseEvent *event){
    mouseButton(event->button(), 2, event->x(), event->y(), event->modifiers());
    //draw();
    }

void BVGLWidget::mouseMoveEvent(QMouseEvent *event){
    mouseMotion(event->x(), event->y(), event->modifiers());
    int change = HIGHLIGHT;
    updateGL();
}


void BVGLWidget::keyPressEvent(QKeyEvent *event){

}

void BVGLWidget::changeView(int change){
    menu_proc(change);
    updateGL();

}

void BVGLWidget::toggleHighlight(){
    changeView(HIGHLIGHT);
}
void BVGLWidget::toggleSmooth(){
    changeView(SMOOTH);
}
void BVGLWidget::toggleMesh(){
    changeView(DRAWMESH);
}
void BVGLWidget::togglePatch(){
    changeView(DRAWPATCH);
}
void BVGLWidget::togglePolyMesh(){
    changeView(DRAWPOLYMESH);
}
void BVGLWidget::togglePolyPatch(){
    changeView(DRAWPOLYPATCH);
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








