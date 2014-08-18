#include "disassaydata.h"
#include "exportobject.h"
#include "ui_disassaydata.h"
#include "exportobject.h"
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlRecord>
#include "exportobject.h"
#include "qprintobject.h"
DisAssayData::DisAssayData(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::DisAssayData)
{

    ui->setupUi(this);
    mModel = new QSqlTableModel();
    ui->treeView->setAlternatingRowColors(true);
    ui->treeView->setSortingEnabled(true);
    ui->treeView->setModel (mModel);
    ui->treeView->setWindowTitle ("test");
    mModel->setTable("AssayData");
    mModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mModel->select();

//    stream << "PRAGMA table_info ('" << "DeviceInfo" << "')";
    QSqlQuery query;
    //QString sql;
    //QTextStream stream(&sql);
    //stream << "SELECT DeviceId from AssayData";
    query.exec("SELECT * from AssayData");
    while(query.next ()){
        QString value = query.value("DeviceId").toString ();
        if (ui->comboBox_DevIndex->findText (value) < 0 ){
            ui->comboBox_DevIndex->addItem (value);
        }
        value = query.value("PipeId").toString ();
        if (ui->comboBox_PipeIndexEnd->findText (value) < 0 ){
            ui->comboBox_PipeIndexEnd->addItem (value);
        }
        if (ui->comboBox_PipeIndexStart->findText(value) < 0 ){
            ui->comboBox_PipeIndexStart->addItem (value);
        }
    }
    QObject::connect(ui->pushButton_Query, SIGNAL(clicked()), this, SLOT(FilterQuery()));
    QObject::connect(ui->pushButton_print, SIGNAL(clicked()), this, SLOT(print()));
    QObject::connect(ui->pushButton_Export, SIGNAL(clicked()), this, SLOT(ExportExcel()));
}

DisAssayData::~DisAssayData()
{
    delete ui;
    delete mModel;
}

void DisAssayData::FilterQuery()
{
    QString filter;
    if (ui->checkBox_DevIndex->isChecked ()){
        filter = QString("DeviceId ='") + ui->comboBox_DevIndex->currentText ()+ "'";
    }
    //TODO filter by datetime
    //filter = ui->dateTimeEdit_Start->
    if (ui->checkBox_PipeIndex->isChecked ()){
        filter += QString(" and PipeType<='") +
                ui->comboBox_PipeIndexStart->currentText ()+ "' and PipeType>='" +
                ui->comboBox_PipeIndexEnd->currentText ()+ "'";
    }
    mModel->setFilter(filter);
    mModel->select();
}
void DisAssayData::DebugInfo ()
{
//Insert into AssayData (DeviceId, AssayTime, PipeID,PipeType, Ch4) values(1,2,22,1,2)
    QSqlQuery query;
    QString sql;
    QTextStream stream(&sql);
    QDateTime time;
    time = QDateTime::currentDateTime ();
    int count = 100;
    while((count--)>=0){
        stream << "Insert into AssayData (DeviceId, AssayTime, PipeId, PipeType, Ch4) values(1,'"
                 << time.toString ("yyyy/M/d/mm:ss") << "',0,00," << qrand()%1000 << ")";
        query.exec(sql);
        sql.clear ();
    }

}
void DisAssayData::UpdateDate ()
{
    QSqlQuery query;
    QString sql;
    QTextStream stream(&sql);
    QDateTime time;
    time = QDateTime::currentDateTime ();
    stream << "Insert into AssayData (DeviceId, AssayTime, PipeId, PipeType, Ch4) values("
             <<qrand()%10 << ",'"<< time.toString ("yyyy/M/d/mm:ss") << "',0,00," << qrand()%1000 << ")";
    query.exec(sql);
    mModel->select();
}
void DisAssayData::ExportToDocument(QString &html)
{
    QSqlQuery query;
    QString sql("PRAGMA table_info ('AssayData')");

    html = "<table border=\"1\" cellspacing=\"0\">\n";
    html += "<tr>";

    query.exec(sql);
    while(query.next ()){//setheader
       html += "<th bgcolor=\"lightgray\"><font size=\"+1\"><b><i>";
       html += query.value ("name").toString ();
       html += "</i></b></font></th>\n";
    }
    html += "</tr>\n";

    int row,col;
    for(row=1; row <mModel->rowCount (); row++){
        html += "<tr>";
        for(col=0; col <mModel->columnCount (); col++){
             html+= "<td>" + mModel->record (row).value (col).toString ()+ "</td>";
        }
        html += "</tr>\n";
    }
    html += "</table>";

    return ;
}
void DisAssayData::ExportExcel()
{
    ExportObject expobj;
    QString html;
    ExportToDocument(html);
    expobj.ExportExcel (html);
}
void DisAssayData::print()
{
    QString headertext =
        "<table width=\"100%\">"
        "  <tr>"
        "    <td align=\"left\"><strong>%1</strong></td>"
        "    <td align=\"right\"><strong>&date;</strong></td>"
        "  </tr>"
        "</table>";

    QString footertext = "<p align=\"right\"><strong>&page;</strong></p>";

   QPrintObject printobj(this);
   QString html;
   QTextDocument doc;
   ExportToDocument(html);
   doc.setHtml (html);
//   expobj.ExportExcel(html);
   printobj.setHeaderSize(10);
   printobj.setFooterSize(10);
   printobj.setDateFormat("MMMM dd yyyy");
   printobj.setHeaderText(headertext.arg("temp"));
   printobj.setFooterText(footertext);
   QString tmp("example print");
   printobj.print(&doc);
}
