#include "Stdafx.h"
#include "mainwindow.h"
#include "qlog.h"
//#define __DEBUG__
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle ("windowsxp");
#ifdef __DEBUG__
    qInstallMessageHandler(MessageOutput);
#endif

    QDir dir;
    QString path= dir.currentPath ();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    path =dir.currentPath () + QString("/memory.db");
    dir.fromNativeSeparators (path);
    db.setDatabaseName("E:\\Qt\\Platform\\db\\platform.db");
    db.open();
    MainWindow w;
    w.show();

    return a.exec();
}
