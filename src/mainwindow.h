#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QActionGroup>
#include <QTabWidget>
#include "qdispgraphic.h"
#include "deviceinfo.h"
#include "disassaydata.h"
#include "qcapture.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void InitTabWidget();
public slots:
    void DevConfig();
    void displayData();
    void displayGraphic();
    void Capture();
    void About();
    void TabWidgetIndexChange(int index);
private:
    Ui::MainWindow *ui;
    QAction *exitAct;
    QAction *disdataAct;
    QAction *disitemsAct;
    QAction *disActGraphic;
    QAction *printAct;
    QAction *printPdfAct;
    QAction *mainAct;
    QAction *ghptaAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QTabWidget *mtabWidget;
    QCapture *mCapture;
    DisAssayData *mDisAssayData;
    DeviceInfo *mDevInfo;
    QDispgraphic *mDispGraphic;
};

#endif // MAINWINDOW_H
