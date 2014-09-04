#include "src\qloading.h"
#include "ui_qloading.h"
#include <QSqlQuery>
#include <setting.h>

QLoading::QLoading(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::QLoading),
    mStatus(DIRECTLOADING),
    mTimerCount(5)
{
    ui->setupUi(this);
    setWindowFlags (windowFlags ()&(~Qt::WindowMaximizeButtonHint));
    InitActions();
//    connect(ui->pushButton_DevConfig, SIGNAL(clicked()), this, SLOT(DevConfig()));
    connect(ui->pushButton_DirectLogin, SIGNAL(clicked()),this, SLOT(DirectLogin()));
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(timeout()));
//    mTimer.start (1000);
}

QLoading::~QLoading()
{
    delete ui;
}
void QLoading::InitActions()
{
    QSqlQuery query;
    query.exec("SELECT DeviceID FROM DeviceInfo");
    while(query.next ()) {
        ui->comboBox->addItem (query.value ("DeviceID").toString ());
    }
    qDebug() <<Setting::GetInstance ().GetValue ("DeviceID").toString ();
    ui->comboBox->setCurrentText (Setting::GetInstance ().GetValue ("DeviceID").toString ());
    connect(ui->comboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(DeviceIDChanged(QString)));
}

void QLoading::timeout()
{
    QString temp("Wait User Select,Timeout ");
    temp += QString::number (mTimerCount);
    this->setWindowTitle (temp);
    if((mTimerCount--) < 0) {
        DirectLogin ();
    }
}
void QLoading::DeviceIDChanged(const QString &text)
{
    Setting::GetInstance ().SetKeyInfo ("DeviceID", text);
}
void QLoading::DevConfig()
{
    mStatus = QLoading::LOADSETTING;
    if (mTimer.isActive ()) {
        mTimer.stop ();
    }
    reject ();
}
void QLoading::DirectLogin()
{
    mStatus = QLoading::DIRECTLOADING;
    if (mTimer.isActive ()) {
        mTimer.stop ();
    }
    reject ();
}
