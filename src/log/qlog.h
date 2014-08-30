#ifndef QLOG_H
#define QLOG_H
#include <QtCore>
#include <QString>
#include <QObject>

class LogFile :public QObject
{
    Q_OBJECT
public:
    LogFile(QObject *parent=0);
    ~LogFile();
    static LogFile &GetInstance()
    {
        static LogFile mInstance;
        return mInstance;
    }

    QString SetupFileName();
    void ReinitFile();
    void Write(QString &log);
    void Write(QtMsgType type, const QMessageLogContext &context, const QString &msg);
signals:
    void LogChanged(const QString &log);
private:
    QString mLog;
    QString mFileName;
    QFile mFile;
    unsigned long mFileLength;
    QMutex mMutex;
};


void MessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
#endif // QLOG_H
