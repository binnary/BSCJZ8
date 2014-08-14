#ifndef EXPORTOBJECT_H
#define EXPORTOBJECT_H

#include <QString>
#include <QWidget>

class ExportObject : public QWidget
{
    Q_OBJECT
public:
    explicit ExportObject();
   ~ExportObject();

public:
    void ExportExcel (QString &html);
    void ExportPdf(QString &html);
};

#endif // EXPORTOBJECT_H
