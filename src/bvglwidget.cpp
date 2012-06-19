#include "bvglwidget.h"
#include "bview.h"

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
    //draw();
    }


void BVGLWidget::mouseMoveEvent(QMouseEvent *event){
    mouseMotion(event->x(), event->y(), event->modifiers());
    updateGL();
}


void BVGLWidget::keyPressEvent(QKeyEvent *event){

}
