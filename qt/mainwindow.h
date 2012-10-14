#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QDockWidget>
#include <QListWidget>
#include <QStringList>
#include <QSplitter>
#include <QLabel>
#include <QScrollArea>
#include <QImage>
#include "twcanvas.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QAction *quitAct, *aboutAct;
    QMenu *fileMenu, *helpMenu;
    QToolBar *fileToolBar, *helpToolBar;
    QTextEdit *edit;
    QDockWidget *dock;
    QListWidget *list;
    QSplitter *splitter;
    TwCanvas *imageLabel; //FIXME:不可以使用QLabel，否则有问题
    QScrollArea *scrollArea;
    double scaleFactor;

private slots:
    void about();
};

#endif // MAINWINDOW_H
