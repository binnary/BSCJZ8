#ifndef QSERIAL_H
#define QSERIAL_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include "qprotocol.h"
//#define DEBUG_SERIAL

class QSerial:public QThread
{
    Q_OBJECT
//    Q_ENUMS(FSM)
public:
    explicit QSerial(QSerialPortInfo info, QObject *parent = 0):
        QThread(parent),
        mPort(new QSerialPort(info)),
        mPortInfo(info)
    {
           connect(this, SIGNAL(started ()), this, SLOT(start()));
    }
//    explicit QSerial(QThread *parent = 0);
    ~QSerial(){
        qDebug() << Q_FUNC_INFO << ":" << __LINE__;
        requestInterruption ();
        wait ();
        if (mPort){
            if(mPort->isOpen ()){
                mPort->close ();
            }
            qDebug() << Q_FUNC_INFO << ":" << __LINE__;
            delete mPort;
            mPort = NULL;
        }
    }
    QByteArray Write(QByteArray data);
    virtual bool PaserPackage(QByteArray &Package, bool fcs_state=true);
    const QSerialPort *GetPortDev(){return mPort;}
    const QSerialPortInfo &GetPortInfo(){return mPortInfo;}
    bool  InitPort();
private slots:
    void isStart(){
        qDebug() << Q_FUNC_INFO;
    }
protected:
    void run();
    QByteArray Generate();
    QByteArray ReadAll();
    QSerialPort *mPort;
    QSerialPortInfo mPortInfo;
    QProtocol mProtocol;
    QMutex mutex;
};
#endif // QSERIAL_H
