#include "deviceinfo.h"
#include "ui_deviceinfo.h"
#include "ui_AddNewDev.h"
#include "setting.h"
#include "qprotocol.h"
#include <QDebug>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QSqlQuery>

static QString gList[MAX_MTYPE] = {"CO", "O2", "PRESS_ABS", "PRESS_SFC", "FLOW", "CO2", "CH4", "TEMP"};
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

    ui->lineEdit_ParamA->setValidator (new QDoubleValidator(-999.0, 999.0, 10, ui->lineEdit_ParamA));
    ui->lineEdit_ParamB->setValidator (new QDoubleValidator(-999.0, 999.0, 10, ui->lineEdit_ParamB));
    ui->lineEdit_ParamC->setValidator (new QDoubleValidator(-999.0, 999.0, 10, ui->lineEdit_ParamC));
    ui->lineEdit_ParamD->setValidator (new QDoubleValidator(-999.0, 999.0, 10, ui->lineEdit_ParamD));
    ui->lineEdit_MeasureInterTime->setValidator (new QIntValidator(0, 255, ui->lineEdit_MeasureInterTime));
    ui->lineEdit_SaveInterTime->setValidator (new QIntValidator(0, 255, ui->lineEdit_SaveInterTime));


    for(int i=0; i< MAX_MTYPE; i++) {
        ui->comboBox_ParamType->addItem (gList[i]);
    }
    mPrevDevType = ui->comboBox_ParamType->currentText ();
    connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(DeviceInfoSelected(QModelIndex)));
    connect(ui->pushButton_ModifyParam, SIGNAL(clicked()), this, SLOT(DevParamInfoModify()));
    connect(ui->comboBox_ParamType,
            SIGNAL(currentTextChanged(QString)),
            this,
            SLOT(ParamTypeTextChanged(QString)));
    connect(ui->pushButton_Add, SIGNAL(clicked()), this, SLOT(DbAdd()));
    connect(ui->pushButton_Del, SIGNAL(clicked()), this, SLOT(DbDel()));
    connect(ui->pushButton_ModifyDevInfo, SIGNAL(clicked()), this, SLOT(DbModify()));

}

DeviceInfo::~DeviceInfo()
{
    delete ui;
    delete mModel;
}

void DeviceInfo::DeviceInfoSelected(const QModelIndex &index)
{
//    qDebug() <<"row" << index.row ()<< "col" <<index.column ();
//    qDebug() << mModel->data(mModel->index(index.row (),0)).toString ();
    mCurrentDevId = mModel->data(mModel->index(index.row (),0)).toString ();
    Setting::GetInstance ().SetKeyInfo ("DeviceId", mCurrentDevId);
    UpdateParamTypeValue(ui->comboBox_ParamType->currentText ());
}
void DeviceInfo::DbAdd()
{
    int rowNum = mModel->rowCount(); //获得表的行数
    mModel->insertRow(rowNum); //添加一行
    AddDevice dev(this);// = new AddDevice(this);
    if (dev.exec () == QDialog::Rejected) {
        return;
    }
    mModel->setData(mModel->index(rowNum,0), dev.ui->lineEdit_devId->text ());
//    mModel->setData(mModel->index(rowNum,0), dev.ui->comboBox_devType->currentText ());
    mModel->setData(mModel->index(rowNum,1), dev.ui->comboBox_devType->currentText());
    mModel->setData(mModel->index(rowNum,2), dev.ui->comboBox_PipeIdType->currentText ());
    QString checkable("0");
    if (dev.ui->checkBox_Resizeable->isChecked ()) {
        checkable = "1";
    }
    mModel->setData(mModel->index(rowNum,3), checkable);
    DbModify();
    mModel->select ();
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
        QString DeviceId = mModel->data(
                               mModel->index(ui->tableView->currentIndex ().row (),0)).toString ();
        QSqlQuery query;
        query.exec(QString("DELETE * FROM Settings WHERE DeviceID='")+DeviceId + QString("'"));
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

void DeviceInfo::ParamTypeTextChanged(const QString &text)
{
    _Modify(mCurrentDevId, mPrevDevType);
    UpdateParamTypeValue(text);
    mPrevDevType = text;
}
void DeviceInfo::DevParamInfoModify()
{
    _Modify(mCurrentDevId,
            ui->comboBox_ParamType->currentText ());
}
void DeviceInfo::_Modify(QString DeviceID, QString MeasureType)
{
//    QString sql("update Settings set  Param_A WHERE DeviceID=1 AND UPDATE");
    QSqlQuery query;
    QString ParamA = QString::number (ui->lineEdit_ParamA->text().toDouble ());
    QString ParamB = QString::number (ui->lineEdit_ParamB->text().toDouble ());
    QString ParamC = QString::number (ui->lineEdit_ParamC->text().toDouble ());
    QString ParamD = QString::number (ui->lineEdit_ParamD->text().toDouble ());
    QString MInterval = QString::number (ui->lineEdit_MeasureInterTime->text().toInt ());
    QString SInterval = QString::number (ui->lineEdit_SaveInterTime->text().toInt ());
    if (!ui->lineEdit_ParamC->text ().size ()) {
        ParamC="1.0" ;
        ui->lineEdit_ParamC->setText (ParamC);
    }

    if (!ui->lineEdit_MeasureInterTime->text ().size ()) {
        MInterval ="255" ;
        ui->lineEdit_MeasureInterTime->setText (ParamC);
    }
    if (!ui->lineEdit_SaveInterTime->text ().size ()) {
        SInterval ="255" ;
        ui->lineEdit_SaveInterTime->setText (ParamC);
    }
    QString sql;
    sql = QString("SELECT COUNT(*) FROM Settings WHERE DeviceID='") +
          DeviceID+QString("' AND MeasureType='") +MeasureType +"'";
    query.exec(sql);
    if ((query.next()) &&
            (0 == query.value ("COUNT(*)").toInt ())) {
        sql.clear ();
        QTextStream stream(&sql);
        stream <<"INSERT INTO Settings( DeviceID, MeasureType, Param_A, Param_B,"
               << "Param_C, Param_D,MInterval, SInterval) VALUES('"
               << DeviceID  << "','" << MeasureType << "','"
               << ParamA << "','" << ParamB << "','"
               << ParamC << "','" << ParamD << "','"
               << MInterval << "','" << SInterval <<"')";
    } else {
        sql = "UPDATE Settings SET Param_A='" + ParamA +
              "', Param_B='"  + ParamB +
              "', Param_C='"  + ParamC +
              "', Param_D='"  + ParamD +
              "' WHERE DeviceID='" + DeviceID +
              "' AND MeasureType='" + MeasureType +"'";
    }
    query.exec(sql);

    sql = "UPDATE Settings SET MInterval='" + MInterval +
          "', SInterval='"  + SInterval +
          "' WHERE DeviceID='" + DeviceID + "'" ;
    query.exec(sql);
}

void DeviceInfo::UpdateParamTypeValue(QString ParamType)
{
    QSqlQuery query;
    QString  sql("SELECT * FROM Settings Where ");
    sql += " DeviceID='" + mCurrentDevId +
           "' AND MeasureType='" + ParamType + "'";
//    qDebug() << sql;
    query.exec (sql);
    if (query.next ()) {
        ui->lineEdit_ParamA->setText (query.value ("Param_A").toString ());
        ui->lineEdit_ParamB->setText (query.value ("Param_B").toString ());
        ui->lineEdit_ParamC->setText (query.value ("Param_C").toString ());
        ui->lineEdit_ParamD->setText (query.value ("Param_D").toString ());
        ui->lineEdit_MeasureInterTime->setText (query.value("MInterval").toString ());
        ui->lineEdit_SaveInterTime->setText (query.value("SInterval").toString ());
    } else {
        ui->lineEdit_ParamA->setText ("0.0");
        ui->lineEdit_ParamB->setText ("0.0");
        ui->lineEdit_ParamC->setText ("1.0");
        ui->lineEdit_ParamD->setText ("0.0");
        ui->lineEdit_MeasureInterTime->setText ("255");
        ui->lineEdit_SaveInterTime->setText ("255");
    }

}
