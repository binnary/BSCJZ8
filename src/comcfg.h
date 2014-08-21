#ifndef COMCFG_H
#define COMCFG_H

#include <QSerialPortInfo>
#include <QDialog>
#include <QSettings>
namespace Ui
{
class ComCfg;
}
class QComConfig
{
public:
    QString GetPort()
    {
        return mPort;
    }
    void SetPort(QString &Port)
    {
        mPort = Port;
    }

    QString GetBaudrate()
    {
        return mBaudrate;
    }
    void SetBaudrate(QString Baudrate)
    {
        mBaudrate = Baudrate;
    }

    QString GetDatabits()
    {
        return mDatabits;
    }
    void SetDatabits(QString databits)
    {
        mDatabits = databits;
    }

    QString GetParity()
    {
        return mParity;
    }
    void SetParity(QString Parity)
    {
        mParity = Parity;
    }

    const QString &GetStopbits()
    {
        return mStopbits;
    }
    void SetStopbits(QString Stopbits)
    {
        mStopbits = Stopbits;
    }

    bool GetDtsr()
    {
        return mDtsr;
    }
    void SetDtsr(bool Dtsr)
    {
        mDtsr = Dtsr;
    }

    bool GetCrts()
    {
        return mCrts;
    }
    void SetCrts(bool Crts)
    {
        mCrts = Crts;
    }

    bool GetXonoff()
    {
        return mXonoff;
    }
    void SetXonoff(bool Xonoff)
    {
        mXonoff = Xonoff;
    }
    const QSerialPortInfo &GetPortInfo()
    {
        return mInfo;
    }
    void SetPortInfo(QSerialPortInfo &Info)
    {
        mInfo = Info;
    }
private:
    QSettings mSettings;
    QString mPort;
    QString mBaudrate;
    QString mDatabits;
    QString mParity;
    QString mStopbits;
    QSerialPortInfo mInfo;
    bool mDtsr;
    bool mCrts;
    bool mXonoff;
};

class ComCfg : public QDialog
{
    Q_OBJECT

public:
    explicit ComCfg(QWidget *parent = 0);
    ~ComCfg();
public slots:
    virtual void accept ();
    virtual void reject ();
private:
    Ui::ComCfg *ui;
};

#endif // COMCFG_H
