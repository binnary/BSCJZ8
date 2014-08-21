#include "exportobject.h"
#include <QSqlQuery>
#include <QtDebug>
#include <QVariant>
#include <QTextEdit>
#include <QTextDocumentWriter>
#include <QFileDialog>
#include <QFile>
#include <QPrinter>
ExportObject::ExportObject(QObject *parent):
    QObject(parent)
{

}
ExportObject::~ExportObject()
{

}
void ExportObject::ExportPdf(QString &html, QString fileName)
{
    QTextDocument doc;
    doc.setHtml (html);
    if(!fileName.isEmpty()) {
        if(QFileInfo(fileName).suffix().isEmpty()) {
            fileName.append(".pdf");
        }
        QPrinter  printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        doc.print(&printer);
    }
}
void ExportObject::ExportExcel (QStringList &html, QString fileName)
{
    QStringList::iterator it = html.begin ();
    int FileCount = 0;
    if (fileName.isEmpty()) {
        return;
    }
    if (html.size ()==1) {
        QTextDocument doc;
        doc.setHtml (*it);
        _ExportExcel (fileName, doc);
        return;
    }
    QFileInfo FileInfo(fileName);
    for (; it != html.end (); ++it) {
        QTextDocument doc;
        doc.setHtml (*it);
        fileName = FileInfo.baseName () +
                   QString("_") +
                   QString::number (FileCount++) +
                   QString(".") +
                   FileInfo.suffix ();
        qDebug() <<fileName;
        _ExportExcel(fileName, doc);
    }
}
void ExportObject::ExportExcel (QString &html, QString fileName)
{
    QTextDocument doc;
    if (fileName.isEmpty()) {
        return;
    }
    doc.setHtml (html);
    _ExportExcel(fileName, doc);

}
void ExportObject::_ExportExcel(QString &fileName, QTextDocument &doc)
{
    if (!(fileName.endsWith(".xls", Qt::CaseInsensitive)||
            fileName.endsWith(".odt", Qt::CaseInsensitive) ||
            fileName.endsWith(".htm", Qt::CaseInsensitive) ||
            fileName.endsWith(".html", Qt::CaseInsensitive)||
            fileName.endsWith(".ods", Qt::CaseInsensitive)) ) {
        if(QFileInfo(fileName).suffix().isEmpty()) {
            fileName.append (".xls");
        }
    }
    if (fileName.endsWith(".odt", Qt::CaseInsensitive)) {
        QTextDocumentWriter writer(fileName);
        writer.write(&doc);
    } else {
        QFile file(fileName);
        if (file.open(QFile::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");
            stream<<doc.toHtml("UTF-8");
        }
    }
}
