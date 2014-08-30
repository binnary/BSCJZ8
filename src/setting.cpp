#include "setting.h"
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QSerialPort>
#include <QTextCodec>
Setting::Setting(QObject *parent) :
    QObject(parent)
{
    QDir dir;
    mIniPath =dir.currentPath () + QString("/config/");
    dir.mkdir (mIniPath);
    mIniPath +=QString("/config.ini");
    mSetting = new QSettings(mIniPath, QSettings::IniFormat);
    mSetting->setIniCodec (QTextCodec::codecForName ("GB2312"));
    if (false == QFile::exists (mIniPath)) {
        qDebug() << "Can not found " << mIniPath << ", Reinit config";
        ReInitDefault();
    }
}
Setting::~Setting()
{
    delete mSetting;
}
void Setting::ReInitDefault()
{
    IniData_t temp;
    temp["Baud"] = QSerialPort::Baud9600;
    temp["Data"] = QSerialPort::Data8;
    temp["Parity"] = QSerialPort::NoParity;
    temp["Stop"] = QSerialPort::OneStop;
    temp["Flow"] = QSerialPort::NoFlowControl;
    SetComInfo (temp);
    SetKeyInfo("LANGUAGE", "language/lang_CH.qm");
}

void Setting::SetKeyInfo(QString key, QVariant val)
{
   mSetting->setValue (key, val);
}
void Setting::SetDevInfo(IniData_t &data)
{
    SetKeyInfo ("DEVICE", data);
}
void Setting::SetComInfo(IniData_t &data)
{
    SetKeyInfo ("COM", data);
}
void Setting::SetKeyInfo(QString key, IniData_t data)
{
    IniData_t::iterator it=data.begin ();
    for (; it != data.end (); ++it) {
        mSetting->setValue (QString("/") + key + QString("/") + it->first, it->second);
    }
}
