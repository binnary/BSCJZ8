#include "disassaydata.h"
#include "exportobject.h"
#include "ui_disassaydata.h"
#include <QProgressDialog>
#include <QFileDialog>
#include <ActiveQt/QAXObject>

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
//    QObject::connect (ui->pushButton_export, SIGNAL(clicked()),this, SLOT(ExportExcel()));
    QObject::connect (ui->pushButton_export, SIGNAL(clicked()),this, SLOT(UpdateDate()));
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
//    QSqlQuery query;
//    QString sql;
//    QTextStream stream(&sql);
//    QDateTime time;
//    time = QDateTime::currentDateTime ();
//    stream << "Insert into AssayData (DeviceId, AssayTime, PipeId, PipeType, Ch4) values(1,'"
//             << time.toString ("yyyy/M/d/mm:ss") << "',0,00," << qrand()%1000 << ")";
//    query.exec(sql);
//    mModel->select();
//    timer.start (1000);
     timer.start(0);
     QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(ExportExcel()));
}
void DisAssayData::ExportExcel ()
{
    QString fileName = QFileDialog::getSaveFileName(this, \
              QObject::tr("Save <a href=\"http://www.it165.net/edu/ebg/\" target=\"_blank\" \
              class=\"keylink\">excel</a>"), \
              ".", tr("Microsoft Office 2003 (*.xls)"));
            //获取保存路径
    if (fileName.isEmpty()) {
        QMessageBox::critical(0, tr("错误"), tr("要保存的文件名为空！"));
        return;
    }
    //建立Excel对象
    QAxObject *excel = new QAxObject("Excel.Application");

    excel->dynamicCall("SetVisible(bool)", true); //如果为了看自己的程序到底怎样工作，可以设置为true  www.it165.net
    excel->setProperty("Visible", false);
    QAxObject *workbooks = excel->querySubObject("WorkBooks");
    workbooks->dynamicCall("Add");
    QAxObject *workbook = excel->querySubObject("ActiveWorkBook");
    QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);
    int count = 0;
    QProgressDialog dlg;
    QObject::connect (this, SIGNAL(signalCount(int)), &dlg, SLOT(setValue(int)));
    dlg.exec ();
    QAxObject *cellX;
    char XX[]={"ABCDEFGHIGKLMNOPQRSTUVWXYZ"};
    qDebug() << "Column: " << mModel->columnCount () << ", Row:" << mModel->rowCount ();
    for(count=1; count <mModel->rowCount ();count++){
        emit signalCount ((count*100)/mModel->rowCount ());
        for(int i=0;i<mModel->columnCount ();i++){
           QString X=XX[i]+QString::number(count);//设置要操作的单元格，如A1
           cellX = worksheet->querySubObject("Range(QVariant, QVariant)",X);//获取单元格
           cellX->dynamicCall("SetValue(const QVariant&)",QVariant(mModel->record (count).value (i)));
        }
    }


    workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(fileName));
    QMessageBox::information(this, tr("OK"), tr("保存成功！"));
    workbook->dynamicCall("Close()");
    worksheet->clear();//释放所有工作表
    excel->dynamicCall("Quit()");
    delete excel;//释放excel
}
