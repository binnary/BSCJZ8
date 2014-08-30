#include "qcapture.h"
#include "qlog.h"
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
#include <QMessageBox>
QCapture::QCapture(QWidget *parent) :
    QWidget(parent),
    mAutoScroll(false),
    mIsStarted(false),
    mPort(NULL)
{
    setupUi (this);
#ifdef Q_OS_WIN
    QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts ();
    QList<QSerialPortInfo>::iterator it;
    for(it= info.begin (); it!=info.end (); ++it) {
        comboBox->addItem (((QSerialPortInfo)*it).portName ());
    }
#endif
    mModel =  new QSqlTableModel();
    MainTreeView->setAlternatingRowColors(true);
    MainTreeView->setSortingEnabled(false);
    MainTreeView->setModel (mModel);
    MainTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    MainTreeView->setWindowTitle ("test");

    mModel->setTable("temp");
    mModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    mModel->database ().exec ("DELETE FROM temp");
    mModel->select();
    connect(&timer, SIGNAL(timeout()), this, SLOT(DebugInfo ()));
    connect(MainTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(AutoScroll()));
    connect(pushButton, SIGNAL(toggled(bool)), this, SLOT(ToggledCapture(bool)), Qt::AutoConnection);
    connect(pushButton_Clear, SIGNAL(clicked()), this, SLOT(ClearData()));
    LogTextEdit->setText ("ssssssssssssssssss");
    connect(&LogFile::GetInstance (), SIGNAL(LogChanged(QString)), LogTextEdit, SLOT(setText(QString)));
    connect(LogTextEdit, SIGNAL(textChanged()), this, SLOT(LogTextEditAutoScroll()));
}

QCapture::~QCapture()
{
    //clear temp table, and release diskspace
    mModel->database ().exec ("DELETE FROM temp;vacuum");
    delete mModel;
}
void QCapture::LogTextEditAutoScroll()
{
    QTextCursor cursor = LogTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    LogTextEdit->setTextCursor(cursor);
}
void QCapture::ClearData()
{
    mModel->database ().exec ("DELETE FROM temp;vacuum");
    qDebug() << mModel->database ().lastError ();
    mModel->select ();
    mModel->removeRows (0, mModel->rowCount ());
}
void QCapture::ToggledCapture(bool toggled)
{
    qDebug() <<toggled;
    if (toggled) {
        Start();
        comboBox->setEnabled (false);
        pushButton->setText (QObject::tr("Stop Read"));
    } else {
        Stop();
        comboBox->setEnabled (true);
        pushButton->setText (QObject::tr("Start Read"));
    }
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
    MainTreeView->scrollToBottom ();

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
    QString port = comboBox->currentText ();
    if (!mPort) {
        return;
    }
    mPort->exit (0);
    //if (mPort->isRunning ()){
    //    mPort->terminate ();
    //}
//    disconnect ()
    delete mPort;
    mPort = NULL;
}
void QCapture::UpdateData(QList<MeasureVal_t> data)
{
//    qDebug() << data;
    foreach(MeasureVal_t var, data) {
        InsterOneItem (var);
    }
    qDebug() << Q_FUNC_INFO;
}
void QCapture::Start ()
{
    qDebug()<< "Qcapture::Start";
    QString port = comboBox->currentText ();
    if (mPort) {
        qWarning() << "Port " << mPort->GetPortInfo ().portName ()<< " is exist";
        mPort->terminate ();
        delete mPort;
        mPort = NULL;
    }
    QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts ();
    QList<QSerialPortInfo>::iterator it;
    for(it= info.begin (); it!=info.end (); ++it) {
        if (port ==((QSerialPortInfo)*it).portName ()) {
            mPort = new QHostPaser(*it);
            break;
        }
    }
    if (!mPort) {
        QMessageBox::warning (this, QObject::tr("Warning"),
                              QObject::tr("Can not found port")
                             );
    }
    connect (mPort, SIGNAL(PackagePaserDone(QList<MeasureVal_t>)), this, SLOT(UpdateData(QList<MeasureVal_t>)));
    connect(pushButton_eraseall, SIGNAL(clicked()), this, SLOT(SendCmdEraseAll()));
    connect(pushButton_setpara, SIGNAL(clicked()), this, SLOT(SendCmdSetPara()));
    connect(pushButton_settime, SIGNAL(clicked()), this, SLOT(SendCmdSetTime()));
    mPort->InitPort ();
    mPort->start ();
}
void QCapture::InsterOneItem(MeasureVal_t &val)
{
    QDateTime time;
    QString  DeviceId, AssayTime, PipeId, PipeType;
    QString Flux, Ch4,Pressure, Comment,LTime,O2,CO2,CO;

    time = QDateTime::currentDateTime ();
    DeviceId = QString::number(qrand()%10);
    AssayTime = time.toString ("yyyy/M/d/mm:ss:zzz");
    PipeType = QString::number(qrand()%10);
    Pressure = QString("%1").arg(val.abs_press);
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
    MainTreeView->scrollToBottom ();

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
}
void QCapture::SendCmdSetTime ()
{
    QProtocol pro;
    QDateTime start=QDateTime::currentDateTime ();
    mPort->Write(pro.makeCmdSetTime (0, start));//, end));
}
void QCapture::SendCmdSetPara()
{
    QProtocol pro;
    QByteArray data("");
    mPort->Write (pro.makeCmdSetParam (0,data));
}
void QCapture::SendCmdEraseAll()
{
    QProtocol pro;
    mPort->Write (pro.makeCmdEraseAll (0));
}
