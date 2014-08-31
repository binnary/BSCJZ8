#include "setting.h"
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QSerialPort>
#include <QTextCodec>
#include <QStringList>

 QString gList[MAX_MTYPE] = {"CO", "O2", "PRESS_ABS", "PRESS_SFC", "FLOW", "CO2", "CH4", "TEMP"};
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

    //language config
    SetKeyInfo("LANGUAGE", "language/lang_CH.qm");

    //Device config
    for(int i=0; i < MAX_MTYPE; ++i){
        mSetting->setValue (gList[i]+"/PARAM_A", 0.0);
        mSetting->setValue (gList[i]+"/PARAM_B", 0.0);
        mSetting->setValue (gList[i]+"/PARAM_C", 1.0);
        mSetting->setValue (gList[i]+"/PARAM_D", 0.0);
    }

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

Settings_t Setting::GetDevSettings(){
    Settings_t set;
     for(int i=0; i < MAX_MTYPE; ++i){
        set.fparam[i].param_A = mSetting->value (gList[i]+"/PARAM_A").toFloat ();
        set.fparam[i].param_B = mSetting->value (gList[i]+"/PARAM_B").toFloat ();
        set.fparam[i].param_C = mSetting->value (gList[i]+"/PARAM_C").toFloat ();
        set.fparam[i].param_D = mSetting->value (gList[i]+"/PARAM_D").toFloat ();
    }
    return set;
}

void Setting::SetDevSettings(Settings_t &set){
//DEVSETTING
// key:MEASURE,
    //setdefault value
    for(int i=0; i < MAX_MTYPE; ++i){
        mSetting->setValue (gList[i]+"/PARAM_A", set.fparam[i].param_A);
        mSetting->setValue (gList[i]+"/PARAM_B", set.fparam[i].param_B);
        mSetting->setValue (gList[i]+"/PARAM_C", set.fparam[i].param_C);
        mSetting->setValue (gList[i]+"/PARAM_D", set.fparam[i].param_D);
    }

}
