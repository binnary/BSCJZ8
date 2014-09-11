#ifndef DISASSAYDATA_H
#define DISASSAYDATA_H

#include <QDialog>
#include <QSqlTableModel>
#include <QTextDocument>
#include <QThread>
#include "exportobject.h"
#include "qprintobject.h"
namespace Ui
{
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
//    void ExportToDocument(QStringList &html, int Split, QString fileName);
    void ExportToDocument(QStringList &html, int Split, QString fileName, bool IsPrint);
//    void ConCurrent(ExportObject *expobj);
signals:
    void signalCount(int count);
    void setDlgText(const QString &text);
    void ExportHtmlDone(const QString &html);
//    void UpdateRecord(QString &html);
public slots:
    void PrintToDevice(const QString &html);
    void FilterQuery();
//    void FilterQuery(const QString &index);
//    void dateTimeChanged(const QDateTime &dateTime);
    void UpdateDate();
    void print();
    void ExportExcel();
private:
    QString ExportToDocumentHeader();
    QString mTableName;
    Ui::DisAssayData *ui;
    QSqlTableModel *mModel;
};

#endif // DISASSAYDATA_H
