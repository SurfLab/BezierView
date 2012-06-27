#ifndef BVGLWIDGET_H
#define BVGLWIDGET_H
#include <QtOpenGL>

class BVGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit BVGLWidget(QWidget *parent = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;


protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void changeView(int change);

signals:
    
public slots:
    void toggleHighlight();
    void toggleSmooth();
    void toggleMesh();
    void togglePatch();
    void togglePolyMesh();
    void togglePolyPatch();
    void toggleCurva();
    void toggleCurvaNeedle();
    void toggleRefline();
};



#endif // BVGLWIDGET_H
