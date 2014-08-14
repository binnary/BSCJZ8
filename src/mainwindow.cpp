#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionPreview_Table, SIGNAL(triggered()), this, SLOT(displayData()));
    connect(ui->actionPreview_Graphic, SIGNAL(triggered()), this, SLOT(displayGraphic()));
    connect(ui->actionCapture, SIGNAL(triggered()), this, SLOT(Capture()));
    connect(ui->action_DevConfig,SIGNAL(triggered()), this, SLOT(DevConfig()));
    Capture ();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Capture()
{
//    mCapture = new QCapture();
    setCentralWidget (new QCapture());
}
void MainWindow::displayData()
{
  DisAssayData *mDisAssayData = new DisAssayData();
//  connect(ui->actionExport_Excel, SIGNAL(triggered()), mDisAssayData, SLOT(print()));
  setCentralWidget (mDisAssayData);
}

void MainWindow::DevConfig()
{
//  DeviceInfo *mDevInfo = new DeviceInfo();
  setCentralWidget (new DeviceInfo());
}

void MainWindow::displayGraphic()
{
//  mDispGraphic = new QDispgraphic;
  setCentralWidget (new QDispgraphic);
}
