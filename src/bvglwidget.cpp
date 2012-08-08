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

void BVGLWidget::resizeGL(int width, int height){
    winWidth = width, winHeight = height;
    glViewport(0,0,winWidth,winHeight);
    updateProjection();
}
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




