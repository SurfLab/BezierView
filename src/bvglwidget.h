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
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    
public slots:

};

#endif // BVGLWIDGET_H
