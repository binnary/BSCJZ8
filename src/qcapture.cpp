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
#include <QMenu>
#include "setting.h"
#include "qprotocol.h"
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

    QSqlQuery query;
    query.exec("SELECT DeviceID FROM DeviceInfo");
    while(query.next ()) {
        comboBox_DeviceID->addItem (query.value ("DeviceID").toString ());
    }
    qDebug() << Setting::GetInstance ().GetValue ("DeviceID").toString ();
    comboBox_DeviceID->setCurrentText (Setting::GetInstance ().GetValue ("DeviceID").toString ());
    connect(comboBox_DeviceID, SIGNAL(currentTextChanged(QString)), this, SLOT(DeviceIDChanged(QString)));

    connect(MainTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(AutoScroll()));
    connect(pushButton, SIGNAL(toggled(bool)), this, SLOT(ToggledCapture(bool)), Qt::AutoConnection);
    connect(pushButton_Clear, SIGNAL(clicked()), this, SLOT(ClearData()));
    connect(LogTextEdit, SIGNAL(textChanged()), this, SLOT(LogTextEditAutoScroll()));
    connect(&LogFile::GetInstance (), SIGNAL(LogChanged(QString)), this, SLOT(LogChanged(QString)));

//  connect(&mTimer, SIGNAL(timeout()), this, SLOT(DebugInfo()));
//    connect(&mTimer, SIGNAL(timeout()), this, SLOT(DebugInfo ()));
//  mTtimer.start (100);
//    QMenu *menu = LogTextEdit->createStandardContextMenu ();
//    QAction *actionAbout = new QAction(LogTextEdit);
//    actionAbout->setObjectName(QStringLiteral("actionAbout"));
//    menu->addAction(actionAbout);
}

QCapture::~QCapture()
{
    //clear temp table, and release diskspace
    mModel->database ().exec ("DELETE FROM temp;vacuum");
    delete mModel;
}
void QCapture::DeviceIDChanged(const QString &text)
{
    Setting::GetInstance ().SetKeyInfo ("DeviceID", text);
}
void QCapture::LogChanged(const QString &text)
{
//   LogText += text;
//   LogTextEdit->setText (LogText);
    LogTextEdit->append (text);
}

void QCapture::LogTextEditAutoScroll()
{
    QTextCursor cursor = LogTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    LogTextEdit->setTextCursor(cursor);
}
void QCapture::ClearData()
{
    LogText.clear ();
    LogTextEdit->setText (LogText);
    mModel->database ().exec ("DELETE FROM temp");
    mModel->select ();
}
void QCapture::ToggledCapture(bool toggled)
{
    if (toggled) {
        Start();
        comboBox->setEnabled (false);
        comboBox_DeviceID->setEnabled (false);
        pushButton->setText (QObject::tr("Stop Read"));
    } else {
        Stop();
        //if(mTimer.isActive ()) {
        //    mTimer.stop ();
        //}
        comboBox->setEnabled (true);
        comboBox_DeviceID->setEnabled (true);
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
    AssayTime = time.toString ("yyyy/M/d/mm:ss");
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
//    mTimer.start (500);
}
void QCapture::Stop ()
{
//    qDebug()<< "Qcapture::stop";
    QString port = comboBox->currentText ();
    if (!mPort) {
        return;
    }
    mPort->exit (0);
    delete mPort;
    mPort = NULL;
}
void QCapture::UpdateData(QList<MeasureVal_t> data)
{
    foreach(MeasureVal_t var, data) {
        InsterOneItem (var);
    }
}
void QCapture::Start ()
{
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
    connect (mPort, SIGNAL(ACK()), this, SLOT(ReceiveACK()), Qt::QueuedConnection);
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
    AssayTime = time.toString ("yyyy/M/d/mm:ss");
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
void QCapture::ReceiveACK()
{
    //if (mTimer.isActive ()) {
    //    mTimer.stop ();
    //}
    pushButton_setpara->setEnabled (true);
    pushButton_settime->setEnabled (true);
    pushButton_eraseall->setEnabled (true);
    pushButton_Clear->setEnabled (true);
    qDebug() << "Receive ACK";
}
void QCapture::PrepareWaitACK()
{
   // pushButton_setpara->setEnabled (false);
   // pushButton_settime->setEnabled (false);
   // pushButton_eraseall->setEnabled (false);
   // pushButton_Clear->setEnabled (false);

//    mWaitAckTime= 10;
//    qDebug() << "Wait for ACK, TimeOut is " << mWaitAckTime << "S";
//    connect(&mTimer, SIGNAL(timeout()), this, SLOT(WaitACK()), Qt::QueuedConnection);
//    mTimer.start (1000);
}
void QCapture::WaitACK()
{
    if ((--mWaitAckTime) >= 0) {
        qDebug() << "Wait for ACK, TimeOut is " <<mWaitAckTime << "S";
//        mTimer.start(1000);
    } else {
        pushButton_setpara->setEnabled (true);
        pushButton_settime->setEnabled (true);
        pushButton_eraseall->setEnabled (true);
        pushButton_Clear->setEnabled (true);
        //if (mTimer.isActive ()) {
        //    mTimer.stop ();
        //}
    }
}
void QCapture::SendCmdSetTime ()
{
    QDateTime start=QDateTime::currentDateTime ();
    mPort->Write(mProtocol.makeCmdSetTime (0, start));//, end));
    PrepareWaitACK();
}
void QCapture::SendCmdSetPara()
{
    Settings_t set = Setting::GetInstance ().GetSettingsT ();
    QByteArray data((char*)&set, sizeof(Settings_t));
    mPort->Write (mProtocol.makeCmdSetParam (0,data));
    PrepareWaitACK();
}
void QCapture::SendCmdEraseAll()
{
    mPort->Write (mProtocol.makeCmdEraseAll (0));
    PrepareWaitACK();
}

