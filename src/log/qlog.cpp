#include "qlog.h"
#include <QFile>
#include <QTextStream>
#include <Qdir>
class LogFile
{
public:
    LogFile()
    {
        //TODO init log file
        //        ReinitFile();
    }
    ~LogFile()
    {
        if (mFile.isOpen ()) {
            mFile.flush();
            mFile.close ();
        }
    }
    QString SetupFileName()
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
    void ReinitFile()
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
    void Write(QString &log)
    {
        QTextStream stream(&mFile);
        if (!mFile.isOpen ()) {
            ReinitFile();
        }
        log.trimmed ();
        stream << log << "\r\n";
        mFile.flush ();
    }
private:
    QString mFileName;
    QFile mFile;
    unsigned long mFileLength;
    QMutex mMutex;
};
static LogFile mLogFile;
void MessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();
    QString log;
    log = QTime::currentTime ().toString ("[hh:mm:ss.zzz]");
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

    log +=QString(context.file) + QString(":");
    log +=QString::number (context.line)+ QString(":");
    log += msg;
    mLogFile.Write (log);
    mutex.unlock ();
}

