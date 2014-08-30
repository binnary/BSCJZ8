#include "comcfg.h"
#include "ui_comcfg.h"

ComCfg::ComCfg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComCfg)
{
    ui->setupUi(this);
}

ComCfg::~ComCfg()
{
    delete ui;
}
void ComCfg::accept ()
{
    //TODO:find user/name
    QDialog::accept ();
}
void ComCfg::reject ()
{
    //TODO:
    QDialog::reject ();
}

