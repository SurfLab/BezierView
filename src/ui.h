#pragma once

class BViewUI : public QGLWidget
{
    Q_OBJECT
public:
    explicit BViewUI(QWidget *parent = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;


    void tryLoadFile(QString fn);
    QMainWindow *mainwindow() const;

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

signals:
    
public slots:
    void command(int);
    void colorDialog();

    void openFile();
    void saveFile();
private:
    QMainWindow *_mainwindow;
    QMenu* _contextMenu, *_menuGroup;
    QSignalMapper *_signalMapper;

    QAction *addMenuAction(QMenu *parent, QString title, int entry, int parameter = 0, const char *shortcut = NULL, bool checkable = false, bool checked = false);
    void createContextMenu();
    void updateMenuAction(int entry, int parameter, bool checked, bool visible = true);
    void updateContextMenu();
};


