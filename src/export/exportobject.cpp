#include "exportobject.h"
#include <QSqlQuery>
#include <QtDebug>
#include <QVariant>
#include <QTextEdit>
#include <QTextDocumentWriter>
#include <QFileDialog>
#include <QFile>
#include <QPrinter>
ExportObject::ExportObject()
{

}
ExportObject::~ExportObject()
{

}
void ExportObject::ExportPdf(QString &html)
{
    QTextDocument doc;
    doc.setHtml (html);
    QString fileName = QFileDialog::getSaveFileName(this,"Export PDF",QString(),"*.pdf");
    if(!fileName.isEmpty()){
         if(QFileInfo(fileName).suffix().isEmpty()){
             fileName.append(".pdf");
         }
         QPrinter  printer(QPrinter::HighResolution);
         printer.setOutputFormat(QPrinter::PdfFormat);
         printer.setOutputFileName(fileName);
         doc.print(&printer);
     }
}
void ExportObject::ExportExcel (QStringList &html)
{
   QStringList::iterator it;
   for (it = html.begin ();it != html.end (); ++it){
       ExportExcel(*it);
   }
}
void ExportObject::ExportExcel (QString &html)
{
    QTextDocument doc;
    doc.setHtml (html);
    QString fileName = QFileDialog::getSaveFileName(this,
                        QObject::tr("Save as..."),
                        QString("a"),
                        QObject::tr("EXCEL files (*.xls);;"
                                    "ODS files (*.ods);;"
                                    "ODF files (*.odt);;"
                                    "HTML-Files (*.htm *.html);;"
                                    "All Files (*)"));
    if (fileName.isEmpty()){
        return;
    }

    if (!(fileName.endsWith(".xls", Qt::CaseInsensitive)||
          fileName.endsWith(".odt", Qt::CaseInsensitive) ||
          fileName.endsWith(".htm", Qt::CaseInsensitive) ||
          fileName.endsWith(".html", Qt::CaseInsensitive)||
          fileName.endsWith(".ods", Qt::CaseInsensitive)) ){
        if(QFileInfo(fileName).suffix().isEmpty()){
            fileName.append (".xls");
        }
    }
    if (fileName.endsWith(".odt", Qt::CaseInsensitive)){
        QTextDocumentWriter writer(fileName);
        writer.write(&doc);
    }else{
        QFile file(fileName);
        if (file.open(QFile::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");
            stream<<doc.toHtml("UTF-8");
        }
    }
}
