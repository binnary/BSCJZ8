#include "qcapture.h"
#include "qlog.h"
#include "dbopt.h"
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
QCapture::QCapture(QWidget *parent) :
    QWidget(parent),
    mAutoScroll(false),
    mRecvData(new QByteArray("")),
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
    for (int i=0; i <mModel->columnCount (); i++) {
//    GetFriendNameByTableColum(QString TableColum)
        mModel->setHeaderData (i, Qt::Horizontal,
                               GetFriendNameByTableColum(
                                   mModel->headerData (i,Qt::Horizontal).toString ()));
    }
    QSqlQuery query;
    //query.exec("SELECT DeviceID FROM DeviceInfo");
    //while(query.next ()) {
    //    comboBox_DeviceID->addItem (query.value ("DeviceID").toString ());
    //}
    query.exec("SELECT DeviceID FROM DeviceInfo");
    while(query.next ()) {
        comboBox_DeviceID->addItem (query.value ("DeviceID").toString ());
    }
    qDebug() << Setting::GetInstance ().GetValue ("DeviceID").toString ();
    comboBox_DeviceID->setCurrentText (Setting::GetInstance ().GetValue ("DeviceID").toString ());
    connect(comboBox_DeviceID, SIGNAL(currentTextChanged(QString)), this, SLOT(DeviceIDChanged(QString)));

    connect(MainTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(AutoScroll()));
    connect(pushButton, SIGNAL(toggled(bool)), this, SLOT(ToggledCapture(bool)), Qt::AutoConnection);
    connect(pushButton_upload, SIGNAL(clicked()), this, SLOT(SendCmdUpload()));
    connect(pushButton_setpara, SIGNAL(clicked()), this, SLOT(SendCmdSetPara()));
    connect(pushButton_settime, SIGNAL(clicked()), this, SLOT(SendCmdSetTime()));
    connect(pushButton_eraseall, SIGNAL(clicked()), this, SLOT(SendCmdEraseAll()));
    connect(pushButton_Clear, SIGNAL(clicked()), this, SLOT(ClearData()));
    connect(LogTextEdit, SIGNAL(textChanged()), this, SLOT(LogTextEditAutoScroll()));
    connect(&LogFile::GetInstance (), SIGNAL(LogChanged(QString)), this, SLOT(LogChanged(QString)));

//    mTimerIdMap[UPDATE_COMPORT_1000] = startTimer (UPDATE_COMPORT_1000);
}

QCapture::~QCapture()
{
    //clear temp table, and release diskspace
    if(mModel) {
        mModel->database ().exec ("DELETE FROM temp;vacuum");
        delete mModel;
    }
    if (mPort) {
        if (mPort->isOpen ()) {
            mPort->close ();
        }
        delete mPort;
        mPort = NULL;
    }
    if (mRecvData) {
        delete mRecvData;
        mRecvData = NULL;
    }
}
void QCapture::timerEvent(QTimerEvent *event)
{
    qDebug() << event->timerId ();
    if (event->timerId () == mTimerIdMap[UPDATE_COMPORT_1000]) {
        QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts ();
        QList<QSerialPortInfo>::iterator it;
        for(it= info.begin (); it!=info.end (); ++it) {
            if (comboBox->findText (((QSerialPortInfo)*it).portName ()) < 0) {
                comboBox->addItem (((QSerialPortInfo)*it).portName ());
            }
        }
        //for(int i=0; i < comboBox->count (); ++i){
        //    bool isFound = true;
        //    for(it= info.begin (); it!=info.end (); ++it) {
        //        if ((((QSerialPortInfo)*it).portName () == comboBox->itemText (i)) >= 0){
        //            isFound =  false;
        //        }
        //    }
        //    if (!isFound){
        //        comboBox->removeItem (i);
        //    }
        //}
    }
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
        mRecvData->clear ();
        Start();
        comboBox->setEnabled (false);
        comboBox_DeviceID->setEnabled (false);
        pushButton->setText (QObject::tr("Close Device"));
    } else {
        Stop();
        //if(mTimer.isActive ()) {
        //    mTimer.stop ();
        //}
        comboBox->setEnabled (true);
        comboBox_DeviceID->setEnabled (true);
        pushButton->setText (QObject::tr("Open Device"));
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
    AssayTime = time.toString ("yyyy/M/d/hh:mm:ss");
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
//    QString port = comboBox->currentText ();
    if (!mPort) {
        return;
    }
//    mPort->exit (0);
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
        delete mPort;
        mPort = NULL;
    }
    QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts ();
    QList<QSerialPortInfo>::iterator it;
    for(it= info.begin (); it!=info.end (); ++it) {
        if (port ==((QSerialPortInfo)*it).portName ()) {
            mPort = new QSerialPort(*it);
            break;
        }
    }
    if (!mPort) {
        QMessageBox::warning (this, QObject::tr("Warning"),
                              QObject::tr("Can not found port")
                             );
    }
    //connect (mPort, SIGNAL(PackagePaserDone(QList<MeasureVal_t>)), this, SLOT(UpdateData(QList<MeasureVal_t>)));
    //connect (mPort, SIGNAL(ACK()), this, SLOT(ReceiveACK()), Qt::QueuedConnection);
    connect(mPort, SIGNAL(readyRead()), this, SLOT(CanReceiveData()));

    Setting &set = Setting::GetInstance ();
    mPort->setBaudRate(set.GetValue ("COM/Baud").toInt(),QSerialPort::AllDirections);
    mPort->setDataBits((QSerialPort::DataBits)set.GetValue ("COM/Data").toInt());
    mPort->setParity((QSerialPort::Parity)set.GetValue ("COM/Parity").toInt ());
    mPort->setStopBits((QSerialPort::StopBits)set.GetValue ("COM/Stop").toInt ());
    mPort->setFlowControl((QSerialPort::FlowControl)set.GetValue ("COM/Flow").toInt ());
    if (!mPort->open (QSerialPort::ReadWrite)) {
        qDebug() << "Open Port failed";
        return;
    }
}
void QCapture::InsterOneItem(MeasureVal_t &val)
{
    QDateTime time;
    QString  DeviceId, AssayTime, PipeId, PipeType;
    QString SfcPressure, AbsPressure,Ch4,O2,CO2,CO, Temperature;

    time = QDateTime::currentDateTime ();
    DeviceId = QString::number(qrand()%10);
    AssayTime = time.toString ("yyyy/M/d/hh:mm:ss");
    PipeType = QString::number(val.pipe_type);
    PipeId = QString::number(val.pipe_num);
    AbsPressure = QString::number (val.abs_press);
    SfcPressure = QString::number (val.sfc_press);
    Ch4 = QString::number(val.CH4);
    O2  = QString::number(val.O2);
    CO2 = QString::number(val.CO2);
    CO  = QString::number(val.CO);
    Temperature = QString::number (val.temperature);

    int row = mModel->rowCount ();
    mModel->insertRow (row);
    mModel->setData (mModel->index (row,0), DeviceId);
    mModel->setData (mModel->index (row,1), AssayTime);
    mModel->setData (mModel->index (row,2), PipeId);
    mModel->setData (mModel->index (row,3), PipeType);
    mModel->setData (mModel->index (row,4), SfcPressure);
    mModel->setData (mModel->index (row,5), AbsPressure);
    mModel->setData (mModel->index (row,6), Temperature);
    mModel->setData (mModel->index (row,6), Ch4);
    mModel->setData (mModel->index (row,9), O2);
    mModel->setData (mModel->index (row,10), CO2);
    mModel->setData (mModel->index (row,11), CO);
    mModel->submitAll ();
    MainTreeView->scrollToBottom ();

    QSqlQuery query;
    QString sql;
    QTextStream stream(&sql);
    stream << "Insert into AssayData(DeviceID, AssayTime, PipeType, PipeId,  "
           << "SfcPressure, AbsPressure,Temperature,Ch4,O2,CO2,CO) values("
           << DeviceId << ",'"  << AssayTime<< "',"
           << PipeType << ","  << PipeId  << "," << SfcPressure << "," <<AbsPressure << ","
           << Temperature << "," << Ch4 << "," << O2 << "," << CO2 << ","
           << CO <<")";
    qDebug() << sql;
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
    mPort->write(mProtocol.makeCmdSetTime (CurrentDevID (), start));//, end));
    PrepareWaitACK();
}
void QCapture::SendCmdSetPara()
{
    Settings_t set = Setting::GetInstance ().GetSettingsT ();
    QByteArray data((char*)&set, sizeof(Settings_t));
    mPort->write (mProtocol.makeCmdSetParam (CurrentDevID (),data));
    PrepareWaitACK();
}
void QCapture::SendCmdEraseAll()
{
    mPort->write (mProtocol.makeCmdEraseAll (CurrentDevID()));
    PrepareWaitACK();
}

void QCapture::SendCmdUpload()
{
//QDateTime::fromString
    Setting &set = Setting::GetInstance ();
    QDate StartDate = QDate::fromString (
                          set.GetValue ("/UpLoad/StartDate").toString (),
                          QString("yyyy/M/d"));
    QDate EndDate = QDate::fromString (
                        set.GetValue ("/UpLoad/EndDate").toString (),
                        QString("yyyy/M/d"));
    mPort->write (mProtocol.makeCmdUpload (CurrentDevID (),StartDate, EndDate));
}
void QCapture::CanReceiveData()
{
    QByteArray Package("");
    QByteArray temp("");
    QProtocol pro;

    temp =  mPort->readAll ();
    if (!temp.size ()) {
        return;
    }
//    qDebug() << "H<--D:"<< pro.DumpArray (temp);
    mRecvData->append(temp);
//  qDebug() <<"RecvData:" << mProtocol.DumpArray (*mRecvData);
// PACKAGE=STX+LEN+ADDR+CMD+DATA+FCS
// LEN = ADDR + CMD+DATA+FCS
//  FCS = STX+LEN+ADDR+CMD+DATA ^
    //i=Current STX
    int STX;
    quint8 PackageLen=0;
    for(STX=0; STX < mRecvData->size()-1; ++STX) {
        if (mRecvData->at(STX) != QProtocol::STX) {
            continue;
        }
        PackageLen = (quint8)mRecvData->at (STX+1);
        if (STX + PackageLen+1 >= mRecvData->size()) {
            //have not more data;
            *mRecvData = mRecvData->mid(STX);
            break;
        }
        //Package=LEN+ADDR+CMD+DATA
        Package = mRecvData->mid(STX, PackageLen+1);
        quint8 fcs = mProtocol.makeFCS(Package);
//        qDebug() <<"fcs=" << fcs << "; pack fsc=" << mRecvData->at(PackageLen+STX+1);
        if (fcs == (quint8)mRecvData->at(PackageLen+STX+1)) {
            PaserPackage(Package);
        } else {
            PaserPackage(Package, false);
        }
        STX += PackageLen+1;
    }
    if (STX==mRecvData->size ()) {
        mRecvData->clear ();
    }
}
bool QCapture::PaserPackage(QByteArray &Package, bool fcs)
{
    QProtocol pro;
    qDebug() << "PACKAGE:" << pro.DumpArray (Package);
    if (!fcs) {
        mPort->write(pro.makeCmdNACK (Package.at(2)));
        qWarning() << "Package check failed, skip this Package";
        return false;
    }
    if (QProtocol::CMD_SET_PARAM == Package.at (3)) {
        mPort->write(pro.makeCmdACK (Package.at(2)));
        qDebug() << "Set Param";
    }
    if (QProtocol::CMD_SET_TIME == Package.at (3)) {
        mPort->write(pro.makeCmdACK (Package.at(2)));
        qDebug() << "Set Time";
    }
    if (QProtocol::CMD_ERASE_ALL == Package.at (3)) {
        mPort->write(pro.makeCmdACK (Package.at(2)));
        qDebug() << "Erase all";
    }
    if (QProtocol::CMD_UPLOAD == Package.at(3)) { //CMD
        mPort->write(pro.makeCmdACK (Package.at(2)));
        //debug();
        mPort->write(mProtocol.makePackage (CurrentDevID (), QProtocol::CMD_UPLOAD_RESP,
                                            mProtocol.makeUploadResp (qrand()%5)));
        mPort->write(mProtocol.makePackage (CurrentDevID (), QProtocol::CMD_UPLOAD_RESP,
                                            mProtocol.makeUploadResp (qrand()%5)));
        //mPort->write(mProtocol.makePackage (CurrentDevID (), QProtocol::CMD_UPLOAD_RESP,
        //                                    mProtocol.makeUploadResp (qrand()%5)));
        //mPort->write(mProtocol.makePackage (CurrentDevID (), QProtocol::CMD_UPLOAD_RESP,
        //                                    mProtocol.makeUploadResp (qrand()%5)));
        //mPort->write(mProtocol.makePackage (CurrentDevID (), QProtocol::CMD_UPLOAD_RESP,
        //                                    mProtocol.makeUploadResp (qrand()%5)));
        qDebug() << "Resquest Upload";

    }
    if (QProtocol::CMD_UPLOAD_RESP == Package.at(3)) { //CMD
        mPort->write(pro.makeCmdACK (Package.at(2)));
        QList<MeasureVal_t> resp =
            mProtocol.PaserRespCmdUpload (Package.mid(4));
        UpdateData(resp);
        qDebug() << "Upload Resp " << resp.size ();
    }
    if (QProtocol::CMD_ACK == Package.at(3)) { //CMD
        qDebug() << "Recv ACK";
    }
    return true;
}
