#ifndef EXPORTOBJECT_H
#define EXPORTOBJECT_H

#include <QString>
#include <QStringList>
#include <QWidget>
#include <QTextDocument>

class ExportObject : public QObject
{
    //    Q_OBJECT
public:
    explicit ExportObject(QObject *parent=0);
    ~ExportObject();

public:
    void ExportExcel (QString &html, QString fileName);
    void ExportExcel (QStringList &html, QString fileName);
    void ExportPdf(QString &html, QString fileName);
private:
    void _ExportExcel(QString &fileName, QTextDocument &doc);
};

#endif // EXPORTOBJECT_H
