#include "qlog.h"
#include <QFile>
#include <QTextStream>
#include <Qdir>

LogFile::LogFile(QObject *parent):
    QObject(parent)
{

}
LogFile::~LogFile()
{
    if (mFile.isOpen ()) {
        mFile.flush();
        mFile.close ();
    }
}

QString LogFile::SetupFileName()
{
    QDir dir;
    QString file;
    QTextStream stream(&file);
    stream <<  dir.currentPath () << QString("\\LOG");
    dir.mkdir (file);
    stream <<QString("\\Platform-");
    stream << QDate::currentDate ().toString ("yyyyddMM");
    //stream << QTime::currentTime ().toString ("hhmmss");
    stream << QString( ".log");

    return file;
}
void LogFile::ReinitFile()
{
    mMutex.lock ();
    if (mFile.isOpen ()) {
        mFile.flush();
        mFile.close ();
    }
    mFileName =SetupFileName();
    mFile.setFileName (mFileName);
    mFile.open (QIODevice::WriteOnly|QIODevice::Unbuffered|QIODevice::Append);
    mMutex.unlock ();
}
//void LogFile::Write(QString &log)
void LogFile::Write(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QTextStream stream(&mFile);
    if (!mFile.isOpen ()) {
        ReinitFile();
    }
    msg.trimmed ();
    QString log;
    log = QTime::currentTime ().toString ("[hh:mm:ss.zzz]");
    mLog += log;
    switch(type) {
    case QtDebugMsg:
        log += QString("Debug: ");
        break;
    case QtWarningMsg:
        log += QString("Warning: ");
        break;
    case QtCriticalMsg:
        log += QString("Critical: ");
        break;
    case QtFatalMsg:
        log += QString("Fatal: ");
        break;
    default:
        log = QString("UnknowMsg: ");
        break;
    };
    log +=QString(context.function) + QString(":");
    log +=QString::number (context.line)+ QString(":");
    log += msg;

    log.trimmed ();
    stream << log << "\r\n";
    mLog += msg + QString("\n");
    emit LogChanged (mLog);
    mFile.flush ();
}

void MessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();
    LogFile::GetInstance ().Write (type, context, msg);
    mutex.unlock ();
}

