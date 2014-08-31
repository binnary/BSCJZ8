#ifndef SETTING_H
#define SETTING_H

#include <QObject>
#include <QString>
#include <QSettings>
#include <QVariant>
#include <map>
#include "qprotocol.h"
typedef std::map<QString, QVariant> IniData_t;

class Setting : public QObject
{
    Q_OBJECT
public:
    explicit Setting(QObject *parent = 0);
    ~Setting();
    static Setting &GetInstance()
    {
        static Setting mInstance;
        return mInstance;
    }
    void ReInitDefault();
    void SetKeyInfo(QString key, IniData_t data);
    void SetKeyInfo(QString key, QVariant val);
    void SetComInfo(IniData_t &data);
    void SetDevInfo(IniData_t &data);
    Settings_t GetDevSettings();
    void SetDevSettings(Settings_t  &set);
    QVariant GetValue(QString key)
    {
        return mSetting->value(key);
    }

private:
    QSettings *mSetting;
    QString mIniPath;
};

#endif // SETTING_H
