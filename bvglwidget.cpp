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
void BVGLWidget::mousePressEvent(QMouseEvent *event){}
void BVGLWidget::mouseMoveEvent(QMouseEvent *event){}
