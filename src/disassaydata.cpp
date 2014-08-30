#include "disassaydata.h"
#include "exportobject.h"
#include "ui_disassaydata.h"
#include "exportobject.h"
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QtConcurrent>
#include <QProgressDialog>
#include <QFileDialog>
DisAssayData::DisAssayData(QDialog *parent) :
    QDialog(parent),
    mTableName("AssayData"),
    ui(new Ui::DisAssayData)
{

    ui->setupUi(this);
    mModel = new QSqlTableModel();
    ui->treeView->setAlternatingRowColors(true);
    ui->treeView->setSortingEnabled(true);
    ui->treeView->setModel (mModel);
    ui->treeView->setWindowTitle ("test");
    mModel->setTable(mTableName);
    mModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mModel->select();

    //    stream << "PRAGMA table_info ('" << "DeviceInfo" << "')";
    QSqlQuery query;
    //QString sql;
    //QTextStream stream(&sql);
    //stream << "SELECT DeviceId from AssayData";
    query.exec("SELECT * from AssayData");
    while(query.next ()) {
        QString value = query.value("DeviceId").toString ();
        if (ui->comboBox_DevIndex->findText (value) < 0 ) {
            ui->comboBox_DevIndex->addItem (value);
        }
        value = query.value("PipeId").toString ();
        if (ui->comboBox_PipeIndexEnd->findText (value) < 0 ) {
            ui->comboBox_PipeIndexEnd->addItem (value);
        }
        if (ui->comboBox_PipeIndexStart->findText(value) < 0 ) {
            ui->comboBox_PipeIndexStart->addItem (value);
        }
    }
    query.exec("SELECT MAX(AssayTime), MIN(AssayTime) FROM AssayData");
    if(query.next ()){
        qDebug() <<"Time=" << query.value ("MIN(AssayTime)").toString ();
        qDebug() <<"Time=" << query.value ("MAX(AssayTime)").toString ();

        ui->dateTimeEdit_Start->setDateTime(
                     QDateTime::fromString (
                        query.value ("MIN(AssayTime)").toString (),
                                     QString("yyyy/M/d/mm:ss:zzz"))
                    );
        ui->dateTimeEdit_End->setDateTime(
                     QDateTime::fromString (
                        query.value ("MAX(AssayTime)").toString (),
                                     QString("yyyy/M/d/mm:ss:zzz"))
                    );
    }

    connect(ui->checkBox_DevIndex,
            SIGNAL(clicked()),
            this,
            SLOT(FilterQuery()));
    connect(ui->comboBox_DevIndex,
           SIGNAL(currentIndexChanged(QString)),
           this,
           SLOT(FilterQuery(QString)));
    connect(ui->checkBox_StartEndDate,
            SIGNAL(clicked()),
            this,
            SLOT(FilterQuery()));
    connect(ui->dateTimeEdit_Start,
            SIGNAL(dateTimeChanged(QDateTime)),
            this,
            SLOT(dateTimeChanged(QDateTime)));
    connect(ui->dateTimeEdit_End,
            SIGNAL(dateTimeChanged(QDateTime)),
            this,
            SLOT(dateTimeChanged(QDateTime)));
    connect(ui->checkBox_PipeIndex,
            SIGNAL(clicked()),
            this,
            SLOT(FilterQuery()));
    connect(ui->comboBox_PipeIndexStart,
           SIGNAL(currentIndexChanged(QString)),
           this,
           SLOT(FilterQuery(QString)));
   connect(ui->comboBox_PipeIndexEnd,
           SIGNAL(currentIndexChanged(QString)),
           this,
           SLOT(FilterQuery(QString)));
    connect(ui->pushButton_print, SIGNAL(clicked()), this, SLOT(print()),Qt::QueuedConnection);
    connect(ui->pushButton_Export, SIGNAL(clicked()), this, SLOT(ExportExcel()),Qt::QueuedConnection);
}

DisAssayData::~DisAssayData()
{
    delete ui;
    delete mModel;
}

void DisAssayData::dateTimeChanged(const QDateTime &dateTime)
{
//   dateTime = dateTime;
   FilterQuery ();
}
void DisAssayData::FilterQuery(const QString &index)
{
//   index = index;
   FilterQuery ();
}
void DisAssayData::FilterQuery()
{
    QString filter;
    if (ui->checkBox_DevIndex->isChecked ()) {
        filter = QString("DeviceId ='") + ui->comboBox_DevIndex->currentText ()+ "'";
    }
    //TODO filter by datetime
    //filter = ui->dateTimeEdit_Start->
    if (ui->checkBox_StartEndDate->isChecked ()){
        if(filter.size ()){
            filter+= QString(" and ");
        }
        filter += QString ("AssayTime>='") +
                  ui->dateTimeEdit_Start->dateTime().toString ("yyyy/M/d/mm:ss:zzz")+
                  QString("' and AssayTime<='") +
                  ui->dateTimeEdit_End->dateTime().toString ("yyyy/M/d/mm:ss:zzz") +
                  QString("'");
    }
    if (ui->checkBox_PipeIndex->isChecked ()) {
        if(filter.size ()){
            filter+= QString(" and ");
        }
        filter += QString("PipeID>='") +
                  ui->comboBox_PipeIndexStart->currentText ()+
                  "' and PipeID<='" +
                  ui->comboBox_PipeIndexEnd->currentText ()+
                  QString("'");
    }
    qDebug() <<  filter;
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
    while((count--)>=0) {
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
QString DisAssayData::ExportToDocumentHeader()
{
    QString header;
    QSqlQuery query;
    QString sql("PRAGMA table_info ('AssayData')");

    header = "<table border=\"1\" cellspacing=\"0\">\n";
    header += "<tr>";

    query.exec(sql);
    while(query.next ()) { //setheader
        header += "<th bgcolor=\"lightgray\"><font size=\"+1\"><b><i>";
        header += query.value ("name").toString ();
        header += "</i></b></font></th>\n";
    }
    header += "</tr>\n";
    return header;
}
void DisAssayData::ExportToDocument(QStringList &html, int Split, QString fileName)
{
    /// split <=0 no split split >=0 split export file
    QString _html= ExportToDocumentHeader ();
    int col, row = 0;
    QSqlQuery query;
    QString sql("SELECT * from ");
    sql += mTableName;
    if (mModel->filter ().size ()) {
        sql += " WHERE " + mModel->filter ();
    }
    qDebug() << sql;
    query.exec (sql);

    while(query.next ()) {
        if((row++)%100==0) {
            emit signalCount (row);
        }
        _html += QString("<tr>");
        for(col=0; col < mModel->columnCount (); col++) {
            _html+= QString("<td>") + query.value(col).toString ()+ QString("</td>");
        }
        _html += QString("</tr>\n");
        if (Split >0) {
            if (row%Split == 0) {
                _html += "</table>";
                html.push_back (_html);
                _html = ExportToDocumentHeader ();
            }
        }
    }
    _html += QString("</table>");
    html.push_back (_html);
    ExportObject expobj(this);
    emit setDlgText (QObject::tr("Export Data to file"));
    expobj.ExportExcel (html, fileName);
    emit setDlgText (QObject::tr("Export Data to file done"));
    emit signalCount (row);
    return ;
}
void DisAssayData::ExportToDocument(QString &html)
{
    int col,row=0;
    qDebug() <<"rowcount=" << mModel->rowCount () << "col=" <<mModel->columnCount ();
    while((row++) <mModel->rowCount ()) {
        if((row)%100==0) {
            emit signalCount (row);
        }
        html += "<tr>";
        for(col=0; col < mModel->columnCount (); col++) {
            html+= "<td>" + mModel->record (row).value (col).toString ()+ "</td>";
        }
        html += "</tr>\n";
        if (mModel->canFetchMore ()) {
            mModel->fetchMore ();
        }
        qDebug() << row;
    }
    emit signalCount (row);
    html += "</table>";
    qDebug() << "done";
    return ;
}

void DisAssayData::ExportExcel()
{
    int recordCount = 0;
    //query all data
    QString sql;
    sql = QString("Select count(*) from ") + mTableName;
    if (mModel->filter ().size ()) {
        sql += " where " +mModel->filter ();
    }
    qDebug() << sql;
    QSqlQuery query;
    query.exec (sql);
    while(query.next ()) {
        qDebug() << query.value ("count(*)").toString ();
        recordCount = query.value("count(*)").toInt ();
    }
    QString fileName = QFileDialog::getSaveFileName(this,
                       QObject::tr("Save as..."),
                       QString(""),
                       QObject::tr("EXCEL files (*.xls);;"
                                   "ODS files (*.ods);;"
                                   "ODF files (*.odt);;"
                                   "HTML-Files (*.htm *.html);;"
                                   "All Files (*)"));
    QStringList html;
    QFuture<void> future = QtConcurrent::run(this,
                           static_cast<void(DisAssayData::*)(QStringList &, int, QString)>(&DisAssayData::ExportToDocument),
                           html, 10000, fileName);
    QProgressDialog dlg(this);
    dlg.setLabelText (QObject::tr("Prepare Data...."));
    dlg.setRange (0,recordCount);
    connect (this, SIGNAL(signalCount(int)),&dlg,SLOT(setValue(int)));
    connect(this,SIGNAL(setDlgText(QString)), &dlg, SLOT(setLabelText(QString)));
    dlg.exec ();
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
    printobj.setHeaderSize(10);
    printobj.setFooterSize(10);
    printobj.setDateFormat("MMMM dd yyyy");
    printobj.setHeaderText(headertext.arg("temp"));
    printobj.setFooterText(footertext);
    QString tmp("example print");
    printobj.print(&doc);
}
