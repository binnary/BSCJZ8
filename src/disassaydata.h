#ifndef DISASSAYDATA_H
#define DISASSAYDATA_H

#include <QDialog>
#include <QSqlTableModel>
#include <QTextDocument>
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
    void ExportToDocument(QString &html);
signals:
    void signalCount(int count);
public slots:
    void FilterQuery();
    void UpdateDate();
    void print();
    void ExportExcel();
private:
    Ui::DisAssayData *ui;
    QSqlTableModel *mModel;
};

#endif // DISASSAYDATA_H
