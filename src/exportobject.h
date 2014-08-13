#ifndef EXPORTOBJECT_H
#define EXPORTOBJECT_H

#include "Stdafx.h"
#include <QObject>
#include <QStringList>
#include <QSqlDatabase>
#include <QTreeView>
#include <QThread>
#include <QProgressDialog>
#include <QFileDialog>
#include <ActiveQt/QAXObject>

class EEO_Field
{
public:
    EEO_Field(const int ic, const QString &sf, const QString &st):
        iCol(ic),sFieldName(sf),sFieldType(st)
    {

    }
    int iCol;
    QString sFieldName, sFieldType;
};
class ExportObject : public QObject
{
    Q_OBJECT
public:
    explicit ExportObject(const QString &filepath, const QString &sheettitle,
                       QTreeView *tableview):m_ExcelFilePath(filepath),
                       m_SheetName(sheettitle), m_TableView(tableview)
    {
//         thread=cre
    }

   ~ExportObject() {
        QSqlDatabase::removeDatabase("excelexport");
    }
public:
    void setOutputFilePath(const QString &spath) {m_ExcelFilePath = spath;}
    void setOutputSheetTitle(const QString &ssheet) {m_SheetName = ssheet;}
    void setTableView(QTreeView *tableview) {m_TableView = tableview;}

    void addField(const int iCol, const QString &fieldname, const QString &fieldtype)
    {
        m_FieldList << new EEO_Field(iCol, fieldname, fieldtype);
    }

    void removeAllFields()
    {
        while (!m_FieldList.isEmpty()) delete m_FieldList.takeFirst();
    }

    int export2Excel();
    void ExportExcel ();

signals:
    void exportedRowCount(int row);

private:
    QString m_ExcelFilePath;
    QString m_SheetName;
    QTreeView *m_TableView;
    QList<EEO_Field *> m_FieldList;
    QThread thead;
};

#endif // EXPORTOBJECT_H
