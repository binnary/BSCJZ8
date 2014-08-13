#include "deviceinfo.h"
#include "ui_deviceinfo.h"

DeviceInfo::DeviceInfo(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::DeviceInfo)
{
    ui->setupUi(this);
    mModel = new QSqlTableModel();
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setModel (mModel);
    ui->tableView->setWindowTitle ("test");
    mModel->setTable("DeviceInfo");
    mModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mModel->select();
    //QSqlQuery query;
    //QString sql;
    //QTextStream stream(&sql);
    //stream << "PRAGMA table_info ('" << "DeviceInfo" << "')";
    //query.exec(sql);
    //int count=0;
    //while(query.next ()){
    //    mModel->setHeaderData((count++), Qt::Horizontal, query.value ("name"));
    //}
    QObject::connect(ui->pushButton_Add, SIGNAL(clicked()), this, SLOT(DbAdd()));
    QObject::connect(ui->pushButton_Del, SIGNAL(clicked()), this, SLOT(DbDel()));
//    QObject::connect(ui->pushButton_Query, SIGNAL(clicked()), DeviceInfo, SLOT(DbQuery()));
    QObject::connect(ui->pushButton_Modify, SIGNAL(clicked()), this, SLOT(DbModify()));

//    QObject::connect (ui->tableView, SIGNAL(clicked ()), this, SLOT(dbclicked()));
}

DeviceInfo::~DeviceInfo()
{
    delete ui;
    delete mModel;
}

void DeviceInfo::DbAdd()
{
   int rowNum = mModel->rowCount(); //获得表的行数
   mModel->insertRow(rowNum); //添加一行
   int id= 10;
   mModel->setData(mModel->index(rowNum,0),id);
   //model->submitAll(); //可以直接提交
}
void DeviceInfo::DbDel()
{
    //get current row;
   int curRow = ui->tableView->currentIndex().row();
   //remove  this row
   mModel->removeRow(curRow);
    int ok= QMessageBox::warning (this,QObject::tr("Delete This line!"),
                                  QObject::tr("Are You sure delete this line?"),
                                  QMessageBox::Yes,QMessageBox::No);
    if(ok == QMessageBox::No){
       mModel->revertAll(); //if NO, revert commit
    }else{
       mModel->submitAll(); //if YES, submit this change
    }
}
void DeviceInfo::DbQuery()
{

}
void DeviceInfo::DbModify()
{
    mModel->database().transaction();
    if (mModel->submitAll()) {
        mModel->database().commit();
    } else {
        mModel->database().rollback();
        QMessageBox::warning(this, QObject::tr("tableMode"),
                             QObject::tr("Db Error: %1").arg(mModel->lastError ().text ()));
    }
}

void DeviceInfo::accept()
{
   this->accept ();

}
void DeviceInfo::reject()
{
    this->reject ();
}
