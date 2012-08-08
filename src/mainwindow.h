 #ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void saveFile();
    void openFile();
    void setTitle(QString fn);
    void tryLoadFile(QString fn);
private:
    Ui::MainWindow *ui;
    void initializeViewMenu();
};

#endif // MAINWINDOW_H
