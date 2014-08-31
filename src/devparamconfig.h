#ifndef DEVCONFIG_H
#define DEVCONFIG_H

#include <QDialog>
#include <QSqlTableModel>

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
private:
    void _Modify(QString DeviceId, QString MeasureType);
    Ui::DevParamConfig *ui;
    QSqlTableModel *mModel;
    QString mPrevDevId;
    QString mPrevMeasureType;
};

#endif // DEVCONFIG_H
