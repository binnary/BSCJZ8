#include "qabout.h"
#include "ui_qabout.h"

QAbout::QAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QAbout)
{
    ui->setupUi(this);
}

QAbout::~QAbout()
{
    delete ui;
}
