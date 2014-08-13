#ifndef DISASSAYDATA_H
#define DISASSAYDATA_H

#include "Stdafx.h"
#include <QModelIndex>
#include <QWidget>
#include <QDialog>
#include <QDockWidget>
#include <QTimer>
namespace Ui {
class DisAssayData;
}

class DisAssayData : public QDialog
{
    Q_OBJECT
public:
    explicit DisAssayData(QDialog *parent = 0);
    ~DisAssayData();
    void DebugInfo();
signals:
    void signalCount(int count);
public slots:
    void FilterQuery();
    void UpdateDate();
    void ExportExcel();
private:
    Ui::DisAssayData *ui;
    QSqlTableModel *mModel;
    QTimer timer;
};

#endif // DISASSAYDATA_H
