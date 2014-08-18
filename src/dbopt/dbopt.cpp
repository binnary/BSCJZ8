#include "dbopt.h"
#include <QDir>
#include <QMessageBox>
#include <QSqlDatabase>
bool db_open(QString path)
{
    QDir dir;
    path= dir.currentPath ();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    path =dir.currentPath () + QString("/memory.db");
    dir.fromNativeSeparators (path);
    db.setDatabaseName("E:\\Qt\\Platform\\db\\platform.db");
//    db.setDatabaseName("db\\platform.db");
    if(!db.open()){
        QMessageBox::warning (NULL,QObject::tr("Platform Db Failed"),
                              QObject::tr("Open database failed"));
        return false;
    }
//    QMessageBox::warning (NULL,"asd", "asfdas");
    return true;
}
