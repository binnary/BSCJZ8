#ifndef DEVCONFIG_H
#define DEVCONFIG_H

#include <QDialog>
#include <QSqlTableModel>
#include "qprotocol.h"
namespace Ui
{
class DevParamConfig;
}

class DevParamConfig : public QDialog
{
    Q_OBJECT

public:
    explicit DevParamConfig(QWidget *parent = 0);
    ~DevParamConfig();
public slots:
    void UpdateUi();
    void Modify();
    void DeviceIdChanged(const QString &text);
    void MeasureTypeChange(const QString &text);
    Settings_t GetSettingsT(int DeviceID);
    void StartDateChange(const QDate &date);
    void EndDateChange(const QDate &date);
private:
    void _Modify(QString DeviceID, QString MeasureType);
    Ui::DevParamConfig *ui;
    QSqlTableModel *mModel;
    QString mPrevDevId;
    QString mPrevMeasureType;
};

#endif // DEVCONFIG_H
