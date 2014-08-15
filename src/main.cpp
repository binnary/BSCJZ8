#include <QApplication>
#include <QDebug>
#include "mainwindow.h"
#include "norwegianwoodstyle.h"
#include <QStyleFactory>
#include <QMessageBox>
#include "qlog.h"
//#define __DEBUG__
int main(int argc, char *argv[])
{
//    Q_INIT_RESOURCE(styles);
    QApplication a(argc, argv);
    //NorwegianWood Windows WindowsXP Fusion WindowsVista
    QApplication::setStyle(QStyleFactory::create("Windows"));
    QApplication::setStyle(("Fusion"));
    QApplication::setStyle(QStyleFactory::create("WindowsXP"));
    QApplication::setStyle(new NorwegianWoodStyle);
    QApplication::setStyle(QStyleFactory::create("WindowsVista"));
    QApplication::setStyle(QStyleFactory::create("Fusion"));
//    qDebug() <<QStyleFactorykeys ();
#ifdef __DEBUG__
    qInstallMessageHandler(MessageOutput);
#endif

    QDir dir;
    QString path= dir.currentPath ();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    path =dir.currentPath () + QString("/memory.db");
    dir.fromNativeSeparators (path);
    db.setDatabaseName("E:\\Qt\\Platform\\db\\platform.db");
//    db.setDatabaseName("db\\platform.db");
    if(!db.open()){
        QMessageBox::warning (NULL,"asd", "asfdas");
    }
//    QMessageBox::warning (NULL,"asd", "asfdas");
    MainWindow w;
    w.show();

    return a.exec();
}
