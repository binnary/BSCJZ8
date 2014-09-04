#include "devparamconfig.h"
#include "ui_devparamconfig.h"
#include <QSqlQuery>
#include <QDebug>
#include <QDoubleValidator>
#include <QIntValidator>

static QString gList[MAX_MTYPE] = {"CO", "O2", "PRESS_ABS", "PRESS_SFC", "FLOW", "CO2", "CH4", "TEMP"};
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
        value = query.value("MInterval").toString ();
        ui->lineEdit_MeasureInterTime->setText(value);
        value = query.value("SInterval").toString ();
        ui->lineEdit_SaveInterTime->setText(value);
    }
    mPrevDevId = ui->comboBox_DevId->currentText ();
    mPrevMeasureType = ui->comboBox_ParamType->currentText ();
    ui->lineEdit_ParamA->setValidator (new QDoubleValidator(-999.0, 999.0, 10, ui->lineEdit_ParamA));
    ui->lineEdit_ParamB->setValidator (new QDoubleValidator(-999.0, 999.0, 10, ui->lineEdit_ParamB));
    ui->lineEdit_ParamC->setValidator (new QDoubleValidator(-999.0, 999.0, 10, ui->lineEdit_ParamC));
    ui->lineEdit_ParamD->setValidator (new QDoubleValidator(-999.0, 999.0, 10, ui->lineEdit_ParamD));
    ui->lineEdit_MeasureInterTime->setValidator (new QIntValidator(0, 255, ui->lineEdit_MeasureInterTime));
    ui->lineEdit_SaveInterTime->setValidator (new QIntValidator(0, 255, ui->lineEdit_SaveInterTime));

    connect (ui->comboBox_DevId, SIGNAL(currentTextChanged(QString)), this, SLOT(DeviceIdChanged(QString)));
    connect (ui->comboBox_ParamType, SIGNAL(currentTextChanged(QString)), this, SLOT(MeasureTypeChange(QString)));
    connect(ui->pushButton_Modfiy, SIGNAL(clicked()), this, SLOT(Modify()));
    UpdateUi();
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
//    qDebug() << "Prev DevID=" << mPrevDevId << "; Prev MeasureType=" << mPrevMeasureType;
    sql += " DeviceID=" + ui->comboBox_DevId->currentText () +
           " AND MeasureType='" + ui->comboBox_ParamType->currentText () +"'";
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
void DevParamConfig::Modify()
{
    _Modify(ui->comboBox_DevId->currentText () ,
            ui->comboBox_ParamType->currentText ());
}
void DevParamConfig::_Modify(QString DeviceID, QString MeasureType)
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
    QString sql = "UPDATE Settings SET Param_A='" + ParamA +
                  "', Param_B='"  + ParamB +
                  "', Param_C='"  + ParamC +
                  "', Param_D='"  + ParamD +
                  "' WHERE DeviceID='" + DeviceID +
                  "' AND MeasureType='" + MeasureType +"'";
//    qDebug() << sql;
    query.exec(sql);

    sql = "UPDATE Settings SET MInterval='" + MInterval +
          "', SInterval='"  + SInterval +
          "' WHERE DeviceID='" + DeviceID + "'" ;
    query.exec(sql);
}

Settings_t DevParamConfig::GetSettingsT(int DeviceID)
{
    Settings_t set;
    QSqlQuery query;
    QString sql = QString("SELECT * from Settings WHERE DeviceID='") + QString::number (DeviceID) +QString("'");
    memset(&set, 0, sizeof(Settings_t));
    for(int i=0; i< MAX_MTYPE; ++i) {
        set.fparam[i].param_C = 1.0;
    }
    set.m_interval = 255;
    set.s_interval = 255;
    set.device_id = DeviceID;

    query.exec(sql);
    while(query.next ()) {
        QString MeastureType = query.value ("MeasureType").toString ();
        set.m_interval = query.value ("MInterval").toInt ();
        set.s_interval = query.value ("SInterval").toInt ();
        for(int i=0; i <MAX_MTYPE; ++i) {
            if(MeastureType == gList[i]) {
                set.fparam[i].param_A = query.value ("Param_A").toFloat ();
                set.fparam[i].param_B = query.value ("Param_B").toFloat ();
                set.fparam[i].param_C = query.value ("Param_C").toFloat ();
                set.fparam[i].param_D = query.value ("Param_D").toFloat ();
            }
        }
    }
    return set;
}
