#include "dbopt.h"
#include <QDir>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QString>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
bool CreateDefaultDb()
{
    QString DeviceINfoTable("CREATE TABLE DeviceInfo ( \
        DeviceID     NVARCHAR( 6 )  NOT NULL UNIQUE,\
        Version      SMALLINT       DEFAULT ( 0 ), \
        PipeIDType   SMALLINT       DEFAULT ( 0 ), \
        IsResizeable BIT            NOT NULL DEFAULT ( 0 ), \
        IsConcern    BIT            NOT NULL  DEFAULT ( 0 ) \
    )");
    QString AssayDataTable("CREATE TABLE AssayData (  \
                DeviceID    NVARCHAR( 6 )    NOT NULL, \
                AssayTime   DATETIME, \
                PipeType    NVARCHAR( 6 )    NOT NULL, \
                PipeID      NVARCHAR( 20 )   NOT NULL, \
                Flux        REAL             DEFAULT ( 0 ), \
                Ch4         REAL             DEFAULT ( 0 ),\
                Pressure    REAL             DEFAULT ( 0 ), \
                Temperature REAL             DEFAULT ( 0 ), \
                Comment     NVARCHAR( 255 ), \
                LjTime      SMALLINT         DEFAULT ( 168 ),\
                O2          REAL             DEFAULT ( 0 ),\
                CO2         REAL             DEFAULT ( 0 ),\
                CO          REAL             DEFAULT ( 0 ) \
            )");
    QString UserInfoTable("CREATE TABLE UserInfo (  \
                UserID    NVARCHAR( 6 )   NOT NULL \
                                          UNIQUE, \
                UserName  NVARCHAR( 20 ), \
                Password  NVARCHAR( 16 ), \
                RoleLevel SMALLINT        DEFAULT ( 10 )  \
            )");
    QString tempTable("CREATE TABLE temp (  \
                DeviceID    NVARCHAR( 6 )    NOT NULL, \
                AssayTime   DATETIME, \
                PipeType    NVARCHAR( 6 )    NOT NULL, \
                PipeID      NVARCHAR( 20 )   NOT NULL, \
                Flux        REAL             DEFAULT ( 0 ), \
                Ch4         REAL             DEFAULT ( 0 ),\
                Pressure    REAL             DEFAULT ( 0 ), \
                Temperature REAL             DEFAULT ( 0 ), \
                Comment     NVARCHAR( 255 ), \
                LjTime      SMALLINT         DEFAULT ( 168 ),\
                O2          REAL             DEFAULT ( 0 ),\
                CO2         REAL             DEFAULT ( 0 ),\
                CO          REAL             DEFAULT ( 0 ) \
            )");
     QString SettingsTable("CREATE TABLE Settings ( \
                DeviceID    NVARCHAR( 6 )  NOT NULL,\
                MeasureType VARCHAR        NOT NULL, \
                Param_A     VARCHAR        NOT NULL \
                                           DEFAULT ( 0.0 ),\
                Param_B     VARCHAR        NOT NULL\
                                           DEFAULT ( 0.0 ),\
                Param_C     VARCHAR        NOT NULL\
                                           DEFAULT ( 1 ),\
                Param_D     VARCHAR        NOT NULL\
                                           DEFAULT ( 0.0 ),\
                MInterval   INT            NOT NULL\
                                           DEFAULT ( 255 ),\
                SInterval   INT            NOT NULL\
                                           DEFAULT ( 255 )\
            )");

    QSqlQuery query;
    if(!query.exec (AssayDataTable)) {
        qDebug() << query.lastError ();
        return false;
    }
    if (!query.exec (tempTable)) {
        qDebug() << query.lastError ();
        return false;
    }
    if (!query.exec (UserInfoTable)) {
        qDebug() << query.lastError ();
        return false;
    }
    if (!query.exec (DeviceINfoTable)) {
        qDebug() << query.lastError ();
        return false;
    }
    if (!query.exec (SettingsTable)) {
        qDebug() << query.lastError ();
        return false;
    }
    return true;
}
bool db_open()
{
    QDir dir;
    QString path= dir.currentPath ();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    path =dir.currentPath () + QString("/db");
    dir.mkdir (path);
    path +=QString("/platform.db");
    dir.fromNativeSeparators (path);

#ifdef QT_DEBUG
    db.setDatabaseName("E:\\Qt\\Platform\\db\\platform.db");
#else
    db.setDatabaseName(path);
    if (false == QFile::exists (path)) {
        if(!db.open()) {
            QMessageBox::warning (NULL,QObject::tr("Platform Db Failed"),
                                  db.lastError ().text ());
            return false;
        }
        if (false == CreateDefaultDb()) {
            return false;
        }
        return true;
    }
#endif
    if(!db.open()) {
        QMessageBox::warning (NULL,QObject::tr("Platform Db Failed"),
                              db.lastError ().text ());
        return false;
    }
    return true;
}
