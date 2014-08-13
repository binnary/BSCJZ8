#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include "Stdafx.h"
#include <QModelIndex>
#include <QWidget>
#include <QDialog>
#include <QDockWidget>

namespace Ui {
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
    void accept();
    void reject();
private:
    Ui::DeviceInfo *ui;
    QSqlTableModel *mModel;
};

#endif // DEVICEINFO_H
