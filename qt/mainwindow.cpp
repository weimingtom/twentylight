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

    this->quitAct = new QAction(tr("�˳�(&Q)"), this);
    this->connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
    this->aboutAct = new QAction(tr("����(&A)"), this);
    this->connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    this->fileMenu = this->menuBar()->addMenu(tr("�ļ�(&F)"));
    this->fileMenu->addAction(this->quitAct);
    this->helpMenu = this->menuBar()->addMenu(tr("����(&A)"));
    this->helpMenu->addAction(this->aboutAct);

    this->fileToolBar = this->addToolBar(tr("�ļ�"));
    this->fileToolBar->addAction(this->quitAct);
    this->helpToolBar = this->addToolBar(tr("����"));
    this->helpToolBar->addAction(this->aboutAct);

    this->statusBar()->showMessage(tr("����"));

#if USE_DOCK
    //this->edit = new QTextEdit(this);
    //this->setCentralWidget(this->edit);

    this->scrollArea = new QScrollArea(this);
    this->scrollArea->setBackgroundRole(QPalette::Dark);

    this->imageLabel = new TwCanvas(this->scrollArea);

    this->scrollArea->setWidget(this->imageLabel);
    this->setCentralWidget(this->scrollArea);

    this->dock = new QDockWidget(tr("ͼ����Ϣ"), this);
    this->dock->setAllowedAreas(Qt::AllDockWidgetAreas);

    this->list = new QListWidget(this->dock);
    this->list->addItems(QStringList()
                        << "1"
                        << "2");
    this->dock->setWidget(this->list);
    this->addDockWidget(Qt::LeftDockWidgetArea, this->dock);
#else
    //���������˳���෴
    //ע�ⲻҪ��©ͷ�ļ�
    //(x)���캯����һ�����븸������ָ��
    //����Ҫ��ʽ����
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
    QMessageBox::about(this, tr("����"),
                       tr("��Ϸ�༭��"));
}
