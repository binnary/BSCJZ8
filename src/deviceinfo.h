#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QModelIndex>
#include <QMessageBox>
#include <QSqlError>
#include <QWidget>
#include <QDialog>
//#include <QDockWidget>
#include <QSqlTableModel>

namespace Ui
{
class DeviceInfo;
}

//    class DeviceInfo: public Ui_DeviceInfo {};
class DeviceInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceInfo(QDialog *parent = 0);
    ~DeviceInfo();
public slots:
    void DbAdd();
    void DbDel();
    void DbQuery();
    void DbModify();
    void DeviceInfoSelected(const QModelIndex &index);
    void DevParamInfoModify();
    void ParamTypeTextChanged(const QString &text);
private:
    void UpdateParamTypeValue(QString ParamType);
    void _Modify(QString DeviceID, QString MeasureType);
    Ui::DeviceInfo *ui;
    QSqlTableModel *mModel;
    QString mCurrentDevId;
    QString mPrevDevType;
};

#endif // DEVICEINFO_H
