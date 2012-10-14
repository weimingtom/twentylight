#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

#define USE_DOCK 1

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
    this->setWindowTitle("Twilight");
    this->resize(800, 600);

    this->quitAct = new QAction(tr("退出(&Q)"), this);
    this->connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
    this->aboutAct = new QAction(tr("关于(&A)"), this);
    this->connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    this->fileMenu = this->menuBar()->addMenu(tr("文件(&F)"));
    this->fileMenu->addAction(this->quitAct);
    this->helpMenu = this->menuBar()->addMenu(tr("关于(&A)"));
    this->helpMenu->addAction(this->aboutAct);

    this->fileToolBar = this->addToolBar(tr("文件"));
    this->fileToolBar->addAction(this->quitAct);
    this->helpToolBar = this->addToolBar(tr("关于"));
    this->helpToolBar->addAction(this->aboutAct);

    this->statusBar()->showMessage(tr("就绪"));

#if USE_DOCK
    //this->edit = new QTextEdit(this);
    //this->setCentralWidget(this->edit);

    this->scrollArea = new QScrollArea(this);
    this->scrollArea->setBackgroundRole(QPalette::Dark);

    this->imageLabel = new TwCanvas(this->scrollArea);

    this->scrollArea->setWidget(this->imageLabel);
    this->setCentralWidget(this->scrollArea);

    this->dock = new QDockWidget(tr("图层信息"), this);
    this->dock->setAllowedAreas(Qt::AllDockWidgetAreas);

    this->list = new QListWidget(this->dock);
    this->list->addItems(QStringList()
                        << "1"
                        << "2");
    this->dock->setWidget(this->list);
    this->addDockWidget(Qt::LeftDockWidgetArea, this->dock);
#else
    //创建与添加顺序相反
    //注意不要遗漏头文件
    //(x)构造函数不一定传入父容器的指针
    //不需要显式析构
    this->splitter = new QSplitter(this);
    this->list = new QListWidget(this->splitter);
    this->list->addItems(QStringList()
                        << "1"
                        << "2");
    this->edit = new QTextEdit(this->splitter);
    this->splitter->addWidget(this->list);
    this->splitter->addWidget(this->edit);
    this->setCentralWidget(this->splitter);
#endif
}

MainWindow::~MainWindow()
{

}

void MainWindow::about()
{
    QMessageBox::about(this, tr("关于"),
                       tr("游戏编辑器"));
}
