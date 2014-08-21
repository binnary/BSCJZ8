#include "deviceinfo.h"
#include "ui_deviceinfo.h"
#include "ui_AddNewDev.h"

namespace Ui
{
class AddNewDevice;
}
class AddDevice:public QDialog
{
public:
    explicit AddDevice(QDialog *parent = 0):
        QDialog(parent),
        ui(new Ui::AddNewDevice)
    {
        ui->setupUi(this);
    }
    ~AddDevice()
    {
        delete ui;
    }

    Ui::AddNewDevice *ui;
};

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
    AddDevice dev(this);// = new AddDevice(this);
    if (dev.exec () == QDialog::Rejected) {
        return;
    }
    mModel->setData(mModel->index(rowNum,0), dev.ui->comboBox_devType->currentText ());
    mModel->setData(mModel->index(rowNum,1), dev.ui->lineEdit_devId->text ());
    mModel->setData(mModel->index(rowNum,2), dev.ui->comboBox_PipeIdType->currentText ());
    QString checkable("0");
    if (dev.ui->checkBox_Resizeable->isChecked ()) {
        checkable = "1";
    }
    mModel->setData(mModel->index(rowNum,3), checkable);
    mModel->submitAll(); //commit
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
    if(ok == QMessageBox::No) {
        mModel->revertAll(); //if NO, revert commit
    } else {
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
