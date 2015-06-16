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
    void contextMenuEvent(QContextMenuEvent *event);
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *w);
    void changeView(int change);
    void changeColor(float change[]);

signals:
    
public slots:
    void command(int);
    void toggleHighlight();
    void toggleSmooth();
    void toggleMesh();
    void togglePatch();
    void togglePolyMesh();
    void togglePolyPatch();
    void toggleCurva();
    void toggleCurvaNeedle();
    void toggleRefline();
    void colorDialog();
    void toggleNormal();

private:
    QMenu* _contextMenu;
    QSignalMapper *_signalMapper;

    QAction *addMenuAction(QMenu *parent, QString title, int data, const char *shortcut = __null, bool checkable = false, bool checked = false);
    void createContextMenu();
    void updateMenuAction(int id, bool checked, bool visible = true);
    void updateContextMenu();
};



#endif // BVGLWIDGET_H
