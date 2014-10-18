#include "paramlevelconfig.h"
#include "QTextStream"
#include "ui_paramlevelconfig.h"
#include <QDebug>
#include <QSqlQuery>
#define CreateSlq(name) \
    QString name=QString("UPDATE FriendlyName SET RangeMax='") + \
                 ui->doubleSpinBox_Max##name->text ()+ \
                 QString("',RangeMin='") + \
                 ui->doubleSpinBox_Min##name->text ()+ \
                 QString("' WHERE Name='") + \
                 QString(#name)+ \
                 QString("'"); \
    query.exec(name);

#define UpDateUi(name) \
    QString name= QString("SELECT * FROM FriendlyName WHERE Name='")+ #name +QString("'");\
    query.exec(name); \
    if (query.next()){ \
       ui->doubleSpinBox_Max##name->setValue (query.value("RangeMax").toDouble());\
       ui->doubleSpinBox_Min##name->setValue (query.value("RangeMin").toDouble());\
    }
ParamLevelConfig::ParamLevelConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParamLevelConfig)
{
    ui->setupUi(this);
    connect(ui->pushButton_OK, SIGNAL(clicked()), this, SLOT(UpdateValueChanged()));
    QSqlQuery query;
    UpDateUi(AbsPressure);
    UpDateUi(SfcPressure);
    UpDateUi(Temperature);
    UpDateUi(Ch4);
    UpDateUi(CO);
    UpDateUi(O2);
    UpDateUi(CO2);
    UpDateUi(Flow);
}

ParamLevelConfig::~ParamLevelConfig()
{
    delete ui;
}

void ParamLevelConfig::UpdateValueChanged()
{
    QSqlQuery query;
    QTextStream temp;
    CreateSlq(AbsPressure);
    CreateSlq(SfcPressure);
    CreateSlq(Temperature);
    CreateSlq(Ch4);
    CreateSlq(CO);
    CreateSlq(O2);
    CreateSlq(CO2);
    CreateSlq(Flow);
}
