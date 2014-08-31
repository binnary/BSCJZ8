#include "devparamconfig.h"
#include "ui_devparamconfig.h"
#include <QSqlQuery>
#include <QDebug>
#include <QDoubleValidator>
DevParamConfig::DevParamConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DevParamConfig)
{
    ui->setupUi(this);
    QSqlQuery query;
    query.exec("SELECT * from Settings");
    while(query.next ()) {
        QString value = query.value("DeviceId").toString ();
        if (ui->comboBox_DevId->findText (value) < 0 ) {
            ui->comboBox_DevId->addItem (value);
        }
        value = query.value("MeasureType").toString ();
        if (ui->comboBox_ParamType->findText (value) < 0 ) {
            ui->comboBox_ParamType->addItem (value);
        }
    }
    mPrevDevId = ui->comboBox_DevId->currentText ();
    mPrevMeasureType = ui->comboBox_ParamType->currentText ();
    UpdateUi();
    ui->lineEdit_ParamA->setValidator (new QDoubleValidator(-999.0, 999.0, 2, ui->lineEdit_ParamA));
    ui->lineEdit_ParamB->setValidator (new QDoubleValidator(-999.0, 999.0, 2, ui->lineEdit_ParamB));
    ui->lineEdit_ParamC->setValidator (new QDoubleValidator(-999.0, 999.0, 2, ui->lineEdit_ParamC));
    ui->lineEdit_ParamD->setValidator (new QDoubleValidator(-999.0, 999.0, 2, ui->lineEdit_ParamD));
    connect (ui->comboBox_DevId, SIGNAL(currentTextChanged(QString)), this, SLOT(DeviceIdChanged(QString)));
    connect (ui->comboBox_ParamType, SIGNAL(currentTextChanged(QString)), this, SLOT(MeasureTypeChange(QString)));
    connect(ui->pushButton_Modfiy, SIGNAL(clicked()), this, SLOT(Modify()));
}

DevParamConfig::~DevParamConfig()
{
    delete ui;
}

void DevParamConfig::DeviceIdChanged(const QString &text)
{
    mPrevDevId = text;
//    _Modify(mPrevDevId, mPrevMeasureType );
    UpdateUi();
}
void DevParamConfig::MeasureTypeChange(const QString &text)
{
    mPrevMeasureType  = text;
//    _Modify(mPrevDevId, mPrevMeasureType );
    UpdateUi();
}
void DevParamConfig::UpdateUi()
{
    QSqlQuery query;
    QString  sql("SELECT * FROM Settings Where ");
    qDebug() << "Prev DevID=" << mPrevDevId << "; Prev MeasureType=" << mPrevMeasureType;
    sql += " DeviceID=" + ui->comboBox_DevId->currentText () +
           " AND MeasureType='" + ui->comboBox_ParamType->currentText () +"'";
    qDebug() << sql;
    query.exec (sql);
    if (query.next ()) {
        ui->lineEdit_ParamA->setText (query.value ("Param_A").toString ());
        ui->lineEdit_ParamB->setText (query.value ("Param_B").toString ());
        ui->lineEdit_ParamC->setText (query.value ("Param_C").toString ());
        ui->lineEdit_ParamD->setText (query.value ("Param_D").toString ());
    }
}
void DevParamConfig::Modify()
{
    _Modify(ui->comboBox_DevId->currentText () ,
            ui->comboBox_ParamType->currentText ());
}
void DevParamConfig::_Modify(QString DeviceId, QString MeasureType)
{
//    QString sql("update Settings set  Param_A WHERE DeviceID=1 AND UPDATE");
    QSqlQuery query;
    QString ParamA = QString::number (ui->lineEdit_ParamA->text().toDouble ());
    QString ParamB = QString::number (ui->lineEdit_ParamB->text().toDouble ());
    QString ParamC = QString::number (ui->lineEdit_ParamC->text().toDouble ());
    QString ParamD = QString::number (ui->lineEdit_ParamD->text().toDouble ());
    if (!ui->lineEdit_ParamC->text ().size ()) {
        ParamC="1.0" ;
        ui->lineEdit_ParamC->setText (ParamC);
    }

    QString sql = "UPDATE Settings SET Param_A='" + ParamA +
                  "', Param_B='"  + ParamB +
                  "', Param_C='"  + ParamC +
                  "', Param_D='"  + ParamD +
                  "' WHERE DeviceID='" + DeviceId +
                  "' AND MeasureType='" + MeasureType +"'";
    qDebug() << sql;
    query.exec(sql);
}
