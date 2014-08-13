#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QActionGroup>
#include "qdispgraphic.h"
#include "deviceinfo.h"

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
    void displayData();
    void displayItems();
    void displayGraphic();
    void printFunc();
    void exportpdf();
    void Capture();
private:
    Ui::MainWindow *ui;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *disdataAct;
    QAction *disitemsAct;
    QAction *disActGraphic;
    QAction *printAct;
    QAction *printPdfAct;
    QAction *mainAct;
    QAction *ghptaAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    //Display mdisp;
    //QDispgraphic mDispGraphic;

};

#endif // MAINWINDOW_H
