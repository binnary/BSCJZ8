#ifndef EXPORTOBJECT_H
#define EXPORTOBJECT_H

#include <QString>
#include <QStringList>
#include <QWidget>

class ExportObject : public QWidget
{
    Q_OBJECT
public:
    explicit ExportObject();
   ~ExportObject();

public:
    void ExportExcel (QString &html);
    void ExportExcel (QStringList &html);
    void ExportPdf(QString &html);
};

#endif // EXPORTOBJECT_H
