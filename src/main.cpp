#include <QApplication>
#include <QDebug>
#include "mainwindow.h"
#include "norwegianwoodstyle.h"
#include <QStyleFactory>
#include <QMessageBox>
#include "qlog.h"
#include <dbopt/dbopt.h>
#include <setting.h>
int main(int argc, char *argv[])
{
//    Q_INIT_RESOURCE(styles);
    QApplication a(argc, argv);
    //NorwegianWood Windows WindowsXP Fusion WindowsVista
    QApplication::setStyle(QStyleFactory::create("Windows"));
    QApplication::setStyle(("Fusion"));
    QApplication::setStyle(QStyleFactory::create("WindowsXP"));
    QApplication::setStyle(QStyleFactory::create("WindowsVista"));
    QApplication::setStyle(QStyleFactory::create("Fusion"));
//    QApplication::setStyle(new NorwegianWoodStyle);
#ifndef QT_DEBUG
    qInstallMessageHandler(MessageOutput);
#endif
    qInstallMessageHandler(MessageOutput);
    qDebug() << "Start---------------------------------------------------";
    Setting::GetInstance ();
    if (!db_open()) {
        return -1;
    }
    MainWindow w;
    w.show();

    return a.exec();
}
