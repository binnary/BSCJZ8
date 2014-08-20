#include "qcapture.h"
#include "ui_capture.h"
#include "QDebug"
#include <QAbstractTableModel>
#include <QSqlQuery>
#include <QTreeWidget>
#include <QDateTime>
#ifdef Q_OS_WIN
#include <QSerialPortInfo>
#endif
#include <QSqlDriver>
#include <QDebug>
#include <QSqlError>
QCapture::QCapture(QWidget *parent) :
    QWidget(parent),
    mAutoScroll(false)
{
    setupUi (this);
#ifdef Q_OS_WIN
    QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts ();
    QList<QSerialPortInfo>::iterator it;
    for(it= info.begin (); it!=info.end (); ++it){
         comboBox->addItem (((QSerialPortInfo)*it).portName ());
    }
#endif
    mModel =  new QSqlTableModel();
    treeView->setAlternatingRowColors(true);
    treeView->setSortingEnabled(true);
    treeView->setModel (mModel);
    treeView->setWindowTitle ("test");

    mModel->setTable("temp");
    mModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mModel->database ().exec ("DELETE FROM temp");
    mModel->select();
    connect(&timer, SIGNAL(timeout()), this, SLOT(DebugInfo ()));
    connect(treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(AutoScroll()));
    connect(pushButton, SIGNAL(toggled(bool)), this, SLOT(ToggledCapture(bool)));
    timer.start (100);
//    DebugInfo();
}

QCapture::~QCapture()
{
    //clear temp table, and release diskspace
    mModel->database ().exec ("DELETE FROM temp;vacuum");
    delete mModel;
}
void QCapture::ToggledCapture(bool clicked)
{
    qDebug() <<clicked;
}
void QCapture::AutoScroll()
{
    mAutoScroll =!mAutoScroll ;
}
void QCapture::DebugInfo ()
{
    QDateTime time;
    QString  DeviceId, AssayTime, PipeId, PipeType;
    QString Flux, Ch4,Pressure, Comment,LTime,O2,CO2,CO;

    time = QDateTime::currentDateTime ();
    DeviceId = QString::number(qrand()%10);
    AssayTime = time.toString ("yyyy/M/d/mm:ss:zzz");
    PipeType = QString::number(qrand()%10);
    Pressure = QString::number(qrand()%90);
    Comment  = QString::number(qrand()%90);
    PipeId = QString::number(qrand()%10);
    LTime = QString::number(qrand()%90);
    Flux  = QString::number(qrand()%50);
    Ch4 = QString::number(qrand()%90);
    O2  = QString::number(qrand()%90);
    CO2 = QString::number(qrand()%90);
    CO  = QString::number(qrand()%90);

   int row = mModel->rowCount ();
   mModel->insertRow (row);
   mModel->setData (mModel->index (row,0), DeviceId);
   mModel->setData (mModel->index (row,1), AssayTime);
   mModel->setData (mModel->index (row,2), PipeId);
   mModel->setData (mModel->index (row,3), PipeType);
   mModel->setData (mModel->index (row,4), Flux);
   mModel->setData (mModel->index (row,5), Ch4);
   mModel->setData (mModel->index (row,6), Pressure);
   mModel->setData (mModel->index (row,7), Comment);
   mModel->setData (mModel->index (row,8), LTime);
   mModel->setData (mModel->index (row,9), O2);
   mModel->setData (mModel->index (row,10), CO2);
   mModel->setData (mModel->index (row,11), CO);
   mModel->submitAll ();
   treeView->scrollToBottom ();

   QSqlQuery query;
   QString sql;
   QTextStream stream(&sql);
   stream << "Insert into AssayData(DeviceId, AssayTime, PipeId, PipeType, "
          << "Flux, Ch4,Pressure,Comment,LjTime,O2,CO2,CO) values("
          << DeviceId << ",'"  << AssayTime<< "',"
          << PipeId << ","  << PipeType  << "," << Pressure << "," <<Comment << ","
          << LTime << "," << Flux << "," << Ch4 << "," << O2 << ","
          << CO2 << "," << CO << ")";
   query.exec(sql);
   timer.start (10);
}
void QCapture::Stop ()
{
   qDebug()<< "Qcapture::stop";
   timer.stop ();
   mModel->database ().exec ("DELETE FROM temp;vacuum");
}
void QCapture::Start ()
{
    qDebug()<< "Qcapture::start";
    timer.start (10);
}
