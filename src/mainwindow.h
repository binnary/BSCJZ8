#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QActionGroup>
#include <QDockWidget>
#include "qdispgraphic.h"
#include "deviceinfo.h"
#include "disassaydata.h"
#include "qcapture.h"

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
    void DevConfig();
    void displayData();
    void displayGraphic();
    void Capture();
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
};

#endif // MAINWINDOW_H
