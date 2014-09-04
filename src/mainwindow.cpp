#include "mainwindow.h"
#include "devparamconfig.h"
#include "qloading.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QDialog>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(About()));

//    QLoading loading;
//    loading.exec ();
//    if (QLoading::LOADSETTING == loading.GetStatus ()){
//        mtabWidget->setCurrentIndex (3);
//    }
    InitTabWidget();
    connect(mtabWidget, SIGNAL(currentChanged(int)), this, SLOT(TabWidgetIndexChange(int)));
    connect(ui->actionSetting, SIGNAL(triggered()), this, SLOT(Setting()));

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::Setting()
{
    DevParamConfig config;
    config.exec ();
}
void MainWindow::InitTabWidget()
{
    mtabWidget = new QTabWidget(this);
    mCapture = new QCapture();
    mtabWidget->addTab (mCapture, "Capture");
    mDisAssayData = new DisAssayData();
    mtabWidget->addTab (mDisAssayData, "DisAssayData");
    mDispGraphic = new QDispgraphic ();
    mtabWidget->addTab (mDispGraphic, "DisGraphic");
    mDevInfo = new DeviceInfo();
    mtabWidget->addTab (mDevInfo, "DeviceInfo");
    setCentralWidget (mtabWidget);
}
void MainWindow::TabWidgetIndexChange(int index)
{
//    (index == 0) ? mCapture->Start (): mCapture->Stop ();
}
void MainWindow::Capture()
{
    mtabWidget->setCurrentIndex (0);
}
void MainWindow::displayData()
{
    mtabWidget->setCurrentIndex (1);
}

void MainWindow::displayGraphic()
{
    mtabWidget->setCurrentIndex (2);
}

void MainWindow::DevConfig()
{
    mtabWidget->setCurrentIndex (3);
}

void MainWindow::About()
{
    //QString strAbouti("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
    //             "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
    //             "p, li { white-space: pre-wrap; }\n"
    //             "</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
    //             "<table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">\n"
    //             "<tr>\n"
    //             "<td>\n"
    //             "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><img src=\":/images/about.png\" /></p></td>\n"
    //             "<td>\n"
    //             "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic; text-decoration: underline;\">based on Qt5.3.1(MinGW 4.8 ,32Bit), Qwt-6.1.0 </span></p>\n"
    //             "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-b"
    //                                     "lock-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic; text-decoration: underline;\">Copyright 2014. All rights resverved</span></p>\n"
    //             "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic; text-decoration: underline;\">If you have any questions, contact me at Email</span></p>\n"
    //             "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic; text-decoration: underline;\">Auth : binnary</span></p>\n"
    //             "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic; text-decoration: underline;\">Email: binnary@foxmail.com<br /></span></p></td></tr></table></body></html>", 0));
    QString str(QApplication::translate("QAbout", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
                                        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
                                        "p, li { white-space: pre-wrap; }\n"
                                        "</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
                                        "<table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">\n"
                                        "<tr>\n"
                                        "<td>\n"
                                        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><img src=\":/images/about.png\" /></p></td>\n"
                                        "<td>\n"
                                        "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic; text-decoration: underline;\">based on Qt5.3.1(MinGW 4.8 ,32Bit), Qwt-6.1.0 </span></p>\n"
                                        "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-b"
                                        "lock-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic; text-decoration: underline;\">Copyright 2014. All rights resverved</span></p>\n"
                                        "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic; text-decoration: underline;\">If you have any questions, contact me at Email</span></p>\n"
                                        "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic; text-decoration: underline;\">Auth : binnary</span></p>\n"
                                        "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic; text-decoration: underline;\">Email: binnary@foxmail.com<br /></span></p></td></tr></table></body></html>", 0));
    QMessageBox::about (this, QObject::tr("About"), str);
    return;
    QDialog about(this);
    about.setObjectName(QStringLiteral("About"));
    about.resize(520, 193);
    QGridLayout gridLayout(&about);// = new QGridLayout(QAbout);
    QLabel label(&about);
    gridLayout.setObjectName(QStringLiteral("gridLayout"));
    label.setObjectName(QStringLiteral("label"));
    label.setTextFormat(Qt::RichText);
    gridLayout.addWidget(&label, 0, 0, 1, 1);
    label.setText(QApplication::translate("QAbout", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
                                          "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
                                          "p, li { white-space: pre-wrap; }\n"
                                          "</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
                                          "<table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">\n"
                                          "<tr>\n"
                                          "<td>\n"
                                          "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><img src=\":/images/about.png\" /></p></td>\n"
                                          "<td>\n"
                                          "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic; text-decoration: underline;\">based on Qt5.3.1(MinGW 4.8 ,32Bit), Qwt-6.1.0 </span></p>\n"
                                          "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-b"
                                          "lock-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic; text-decoration: underline;\">Copyright 2014. All rights resverved</span></p>\n"
                                          "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic; text-decoration: underline;\">If you have any questions, contact me at Email</span></p>\n"
                                          "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic; text-decoration: underline;\">Auth : binnary</span></p>\n"
                                          "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic; text-decoration: underline;\">Email: binnary@foxmail.com<br /></span></p></td></tr></table></body></html>", 0));
    about.exec ();
}
