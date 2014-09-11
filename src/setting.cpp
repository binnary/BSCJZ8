#include "setting.h"
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QSerialPort>
#include <QTextCodec>
#include <QStringList>
#include <QSqlQuery>
#include <QDate>

static QString gList[MAX_MTYPE] = {"CO", "O2", "PRESS_ABS", "PRESS_SFC", "FLOW", "CO2", "CH4", "TEMP"};
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
    SetKeyInfo("LANGUAGE", "language/lang_CN.qm");

    //Device config
    //for(int i=0; i < MAX_MTYPE; ++i){
    //    mSetting->setValue (gList[i]+"/PARAM_A", 0.0);
    //    mSetting->setValue (gList[i]+"/PARAM_B", 0.0);
    //    mSetting->setValue (gList[i]+"/PARAM_C", 1.0);
    //    mSetting->setValue (gList[i]+"/PARAM_D", 0.0);
    //}
    mSetting->setValue ("DeviceID", "0");
    mSetting->setValue ("/UpLoad/StartDate", QDate::currentDate ().toString ("yyyy/M/d"));
    mSetting->setValue ("/UpLoad/EndDate", QDate::currentDate ().toString ("yyyy/M/d"));
    mSetting->sync ();
}

void Setting::SetKeyInfo(QString key, QVariant val)
{
    mSetting->setValue (key, val);
    mSetting->sync ();
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
    mSetting->sync ();
}

Settings_t Setting::GetDevSettings()
{
    Settings_t set;
    for(int i=0; i < MAX_MTYPE; ++i) {
        set.fparam[i].param_A = mSetting->value (gList[i]+"/PARAM_A").toFloat ();
        set.fparam[i].param_B = mSetting->value (gList[i]+"/PARAM_B").toFloat ();
        set.fparam[i].param_C = mSetting->value (gList[i]+"/PARAM_C").toFloat ();
        set.fparam[i].param_D = mSetting->value (gList[i]+"/PARAM_D").toFloat ();
    }
    return set;
}

void Setting::SetDevSettings(Settings_t &set)
{
//DEVSETTING
// key:MEASURE,
    //setdefault value
    for(int i=0; i < MAX_MTYPE; ++i) {
        mSetting->setValue (gList[i]+"/PARAM_A", set.fparam[i].param_A);
        mSetting->setValue (gList[i]+"/PARAM_B", set.fparam[i].param_B);
        mSetting->setValue (gList[i]+"/PARAM_C", set.fparam[i].param_C);
        mSetting->setValue (gList[i]+"/PARAM_D", set.fparam[i].param_D);
    }
    mSetting->sync ();

}
Settings_t Setting::GetSettingsT()
{
    return GetSettingsT(mSetting->value ("DeviceId").toInt ());
}
Settings_t Setting::GetSettingsT(int DeviceID)
{
    Settings_t set;
    QSqlQuery query;
    QString sql = QString("SELECT * from Settings WHERE DeviceID='") + QString::number (DeviceID) +QString("'");
    memset(&set, 0, sizeof(Settings_t));
    for(int i=0; i< MAX_MTYPE; ++i) {
        set.fparam[i].param_C = 1.0;
    }
    set.m_interval = 255;
    set.s_interval = 255;
    set.device_id = DeviceID;

    query.exec(sql);
    while(query.next ()) {
        QString MeastureType = query.value ("MeasureType").toString ();
        set.m_interval = query.value ("MInterval").toInt ();
        set.s_interval = query.value ("SInterval").toInt ();
        for(int i=0; i <MAX_MTYPE; ++i) {
            if(MeastureType == gList[i]) {
                set.fparam[i].param_A = query.value ("Param_A").toFloat ();
                set.fparam[i].param_B = query.value ("Param_B").toFloat ();
                set.fparam[i].param_C = query.value ("Param_C").toFloat ();
                set.fparam[i].param_D = query.value ("Param_D").toFloat ();
            }
        }
    }
    return set;
}
