#include "qlogin.h"
#include "ui_qlogin.h"

QLogin::QLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QLogin)
{
    ui->setupUi(this);
    mLogin = false;

}

QLogin::~QLogin()
{
    delete ui;
}
