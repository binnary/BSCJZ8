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
    mDataPackageCount(0),
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
        mModel->setHeaderData (i, Qt::Horizontal,
                               GetFriendNameByTableColum(
                                   mModel->headerData (i,Qt::Horizontal).toString ()));
    }
    QSqlQuery query;
    query.exec("SELECT DeviceID FROM DeviceInfo");
    while(query.next ()) {
        comboBox_DeviceID->addItem (query.value ("DeviceID").toString ());
    }
    if (comboBox_DeviceID->findText (
                Setting::GetInstance ().GetValue ("DeviceID").toString ()
            ) < 0) {
        Setting::GetInstance ().SetKeyInfo ("DeviceID", comboBox_DeviceID->currentText ());
    }
    comboBox_DeviceID->setCurrentText (Setting::GetInstance ().GetValue ("DeviceID").toString ());
    connect(comboBox_DeviceID, SIGNAL(currentTextChanged(QString)), this, SLOT(DeviceIDChanged(QString)));

    connect(MainTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(AutoScroll()));
    connect(pushButton, SIGNAL(toggled(bool)), this, SLOT(ToggledCapture(bool)), Qt::AutoConnection);
    connect(pushButton_upload, SIGNAL(toggled(bool)), this, SLOT(ToggledCmdUpload(bool)), Qt::AutoConnection);
//    connect(pushButton_upload, SIGNAL(clicked()), this, SLOT(SendCmdUpload()));
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
    qDebug() << "Timer event: " <<event->timerId ();
    if (event->timerId () == mTimerIdMap[WAIT_ACK_TIMEOUT]) {
        ReceiveACK (QProtocol::CMD_UNKNOW,false);
    }
    if (event->timerId () == mTimerIdMap[WAIT_UPLOAD_PACKAGE]) {
        qDebug() << "Wait Up Load Package Timeout";
        RemoveTimer (WAIT_UPLOAD_PACKAGE);
    }
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
        comboBox->setEnabled (true);
        comboBox_DeviceID->setEnabled (true);
        pushButton->setText (QObject::tr("Open Device"));
    }
}

void QCapture::ToggledCmdUpload(bool toggled)
{
    if (toggled) {
        Setting &set = Setting::GetInstance ();
        QDate StartDate = QDate::fromString (
                              set.GetValue ("/UpLoad/StartDate").toString (),
                              QString("yyyy/M/d"));
        QDate EndDate = QDate::fromString (
                            set.GetValue ("/UpLoad/EndDate").toString (),
                            QString("yyyy/M/d"));
        mPort->write (mProtocol.makeCmdUploadQuery(CurrentDevID (),StartDate, EndDate));
        PrepareWaitACK(STATUS_WAIT_UPLOAD_QUERY);
    } else {
        QByteArray data;
        data.append ((quint8)QProtocol::CMD_UPLOAD);
        data.append (2);
        mPort->write (mProtocol.makePackage(CurrentDevID (),QProtocol::CMD_ACK,
                                            data));
    }
}
void QCapture::AutoScroll()
{
    mAutoScroll =!mAutoScroll ;
}
void QCapture::Stop ()
{
    if (!mPort) {
        return;
    }
    delete mPort;
    mPort = NULL;
}
void QCapture::UpdateData(quint8 DeviceID, QList<MeasureVal_t> data)
{
    int leavePackages = mDataPackageCount - data.size ();
    foreach(MeasureVal_t var, data) {
        InsterOneItem (DeviceID, var);
    }

    if (leavePackages < 0 ) {
        qWarning() << "Receive Larger package";
    } else if(leavePackages == 0) {
//           pushButton_upload->toggled (false);
    } else {
        qWarning() <<QObject::tr("Receive Package ") << data.size ()
                   << QObject::tr(", Remainder ") << leavePackages
                   << QObject::tr(" Package for Receive");
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

    Setting &set = Setting::GetInstance ();
    mPort->setBaudRate(set.GetValue ("COM/Baud").toInt(),QSerialPort::AllDirections);
    mPort->setDataBits((QSerialPort::DataBits)set.GetValue ("COM/Data").toInt());
    mPort->setParity((QSerialPort::Parity)set.GetValue ("COM/Parity").toInt ());
    mPort->setStopBits((QSerialPort::StopBits)set.GetValue ("COM/Stop").toInt ());
    mPort->setFlowControl((QSerialPort::FlowControl)set.GetValue ("COM/Flow").toInt ());

    connect(mPort, SIGNAL(readyRead()), this, SLOT(CanReceiveData()));

    if (!mPort->open (QSerialPort::ReadWrite)) {
        qDebug() << "Open Port failed";
        return;
    }
}
void QCapture::InsterOneItem(quint8 DeviceID, MeasureVal_t &val)
{
    QDateTime time;
    QString  DeviceId, AssayTime, PipeId, PipeType;
    QString SfcPressure, AbsPressure,Ch4,O2,CO2,CO, Temperature;

    time = QDateTime::currentDateTime ();
    DeviceId = QString::number(DeviceID);
    AssayTime = QString("%1/%2/%3/%4:%5:%6")
                .arg(val.cp_time.year+2000)
                .arg(val.cp_time.mon)
                .arg(val.cp_time.mday)
                .arg(val.cp_time.hour)
                .arg(val.cp_time.min)
                .arg(val.cp_time.sec);
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
    if(!query.exec(sql)) {
        qDebug() << sql;
        qDebug()<< "Exec sql failed, Error Info:" << query.lastError ().text ();
    }
}
void QCapture::ReceiveACK(quint32 Cmd, bool isReceiveAck)
{
    QString log;
    pushButton_setpara->setEnabled (true);
    pushButton_settime->setEnabled (true);
    pushButton_eraseall->setEnabled (true);
    pushButton_Clear->setEnabled (true);
    pushButton_upload->setEnabled (true);
    switch(Cmd) {
    case QProtocol::CMD_SET_PARAM:
        log = QObject::tr("Set Param");
        break;
    case QProtocol::CMD_SET_TIME:
        log = QObject::tr("Set Time");
        break;
    case QProtocol::CMD_ERASE_ALL:
        log = QObject::tr("Erase All Device Data");
        break;
    case QProtocol::CMD_UPLOAD_QUERY:
        log = QObject::tr("Receive UPLoad Query");
        break;
    case QProtocol::CMD_UPLOAD:
        log = QObject::tr("Upload done");
        if (pushButton_upload->isChecked ()) {
            pushButton_upload->toggle ();
        }
    default:
        break;
    }
    if (isReceiveAck) {
        log += QObject::tr(" OK") ;
    } else {
        log += QObject::tr(" Failed") ;
    }
    RemoveTimer (WAIT_ACK_TIMEOUT);
    qWarning() << log;
}
void QCapture::InsertTimer(int timerIdKey)
{
    mMutex.lock();
    if (mTimerIdMap.find(timerIdKey) != mTimerIdMap.end ()) {
        killTimer (mTimerIdMap[timerIdKey]);
        mTimerIdMap.remove (timerIdKey);
        qDebug() << "Timer is exist, first remove it";
    }
    mTimerIdMap[timerIdKey] = startTimer (timerIdKey);
    mMutex.unlock();
}
void QCapture::RemoveTimer(int timerIdKey)
{
    mMutex.lock();
    if (mTimerIdMap.find(timerIdKey) != mTimerIdMap.end ()) {
        killTimer (mTimerIdMap[timerIdKey]);
        mTimerIdMap.remove (timerIdKey);
    }
    mMutex.unlock();
}
void QCapture::PrepareWaitACK(Status_enum status)
{
    mStatus = status;
    pushButton_upload->setEnabled (false);
    pushButton_setpara->setEnabled (false);
    pushButton_settime->setEnabled (false);
    pushButton_eraseall->setEnabled (false);
    pushButton_Clear->setEnabled (false);
    InsertTimer(WAIT_ACK_TIMEOUT);
}

void QCapture::SendCmdSetTime ()
{
    QDateTime start=QDateTime::currentDateTime ();
    mPort->write(mProtocol.makeCmdSetTime (CurrentDevID (), start));
    PrepareWaitACK(STATUS_WAIT_SETTIME_ACK);
}
void QCapture::SendCmdSetPara()
{
    Settings_t set = Setting::GetInstance ().GetSettingsT ();
    QByteArray data((char*)&set, sizeof(Settings_t));
    mPort->write (mProtocol.makeCmdSetParam (CurrentDevID (),data));
    PrepareWaitACK(STATUS_WAIT_SETTIME_ACK);
}
void QCapture::SendCmdEraseAll()
{
    mPort->write (mProtocol.makeCmdEraseAll (CurrentDevID()));
    PrepareWaitACK(STATUS_WAIT_ERASEALL_ACK);
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
    PrepareWaitACK(STATUS_WAIT_PREPARE_UPLOAD);
}
void QCapture::CanReceiveData()
{
    QByteArray Package("");
    QByteArray temp("");
    QProtocol mProtocol;

    temp =  mPort->readAll ();
    if (!temp.size ()) {
        return;
    }
//    qDebug() << "H<--D:"<< mProtocol.DumpArray (temp);
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
bool QCapture::HostPaserPackage (QByteArray &Package, bool fcs)
{
    qWarning() << "Host PACKAGE:" << mProtocol.DumpArray (Package);
    if (!fcs) {
        mPort->write(mProtocol.makeCmdNACK (Package.at(3), Package.at(2)));
        qWarning() << "Package check failed, skip this Package";
        return false;
    }
    switch(Package.at(3)) {
    case QProtocol::CMD_ACK: {
        ReceiveACK (Package.at(4),true);
        break;
    }
    case QProtocol::CMD_UPLOAD_QUERY: {
        //STX＋LEN+ ADDR + 0x11 + DATA(4 Byte) + FCS
        mDataPackageCount = Package.mid (4).toUInt ();
        memcpy(&mDataPackageCount, Package.mid(4).data(), sizeof(mDataPackageCount));
        ReceiveACK (Package.at(3),true);
        qWarning() << "Prepare Receive Package Count is " << mDataPackageCount;
        if (mDataPackageCount !=0) {
            SendCmdUpload();
        } else if (0xFFFFFFFF == mDataPackageCount) {
            qWarning() << "Device ERROR";
        }
        break;
    }
    case QProtocol::CMD_UPLOAD: {
        QList<MeasureVal_t> resp = mProtocol.PaserRespCmdUpload (Package.mid(8));
        quint32 PackageNumber;
        memcpy(&PackageNumber, Package.mid(4), sizeof(PackageNumber));
        mPort->write(mProtocol.makeCmdACK (Package.at(3), Package.at(2)));
        UpdateData(Package.at(2), resp);
        qWarning() << "Receive Package, PackageNumber " << PackageNumber << "PackageCount " << resp.size ();
        break;
    }
    default: {
        qWarning() << "Unkow package id " << QString::number (Package.at (3));
        break;
    }
    }

    return true;
}
bool QCapture::ClientPaserPackage (QByteArray &Package, bool fcs)
{
    static int CurrentPackageNumber = 0;
    qWarning() << "Client PACKAGE:" << mProtocol.DumpArray (Package);
    if (!fcs) {
        mPort->write(mProtocol.makeCmdNACK (Package.at(3), Package.at(2)));
        qWarning() << "Package check failed, skip this Package";
        return false;
    }
    switch(Package.at(3)) {
    case QProtocol::CMD_SET_PARAM: {
        qDebug() << "Set Param";
        mPort->write(mProtocol.makeCmdACK (Package.at(3),Package.at (2)));
        break;
    }
    case QProtocol::CMD_SET_TIME: {
        qDebug() << "Set Time";
        mPort->write(mProtocol.makeCmdACK (Package.at(3),Package.at (2)));
        break;
    }
    case QProtocol::CMD_ERASE_ALL: {
        qDebug() << "Erase all";
        mPort->write(mProtocol.makeCmdACK (Package.at(3),Package.at (2)));
        break;
    }
    case QProtocol::CMD_UPLOAD_QUERY: {
        CurrentPackageNumber = 0;
        mDataPackageCount = qrand()%20;
        if (mDataPackageCount==0) mDataPackageCount = 9;
        QByteArray data((char*)&mDataPackageCount, sizeof(quint32));
        data =mProtocol.makePackage (Package.at(2), (quint8)QProtocol::CMD_UPLOAD_QUERY, data);
        mPort->write(data);
        break;
    }
    case QProtocol::CMD_UPLOAD: {
        mPort->write(mProtocol.makePackage (CurrentDevID (), QProtocol::CMD_UPLOAD,
                                            mProtocol.makeUploadResp(5,CurrentPackageNumber)));
        CurrentPackageNumber +=1;
        qWarning() << "Receive Reuqest Upload";
        break;
    }
    case QProtocol::CMD_ACK: {
//                0x2 0x5 0x1 0x6 0x11 0x0
        if (Package.at(4) != QProtocol::CMD_UPLOAD) {
            break;
        }
        if (Package.at (5) == 2) {
            qWarning() << "All Data Upload";
            break;
        }
        qWarning() << "mDataPackageCount=" << mDataPackageCount;
        qWarning() << "CurrentPackageNumber=" << CurrentPackageNumber;
        if(mDataPackageCount - CurrentPackageNumber >= 0) {
            mPort->write(mProtocol.makePackage (
                             CurrentDevID (), QProtocol::CMD_UPLOAD,
                             mProtocol.makeUploadResp(5,CurrentPackageNumber)));
            CurrentPackageNumber +=1;
        }
        if (mDataPackageCount == CurrentPackageNumber) {
            mPort->write(mProtocol.makeCmdACK (QProtocol::CMD_UPLOAD,Package.at (2)));
        }
        break;
    }
    }
    return true;
}
bool QCapture::PaserPackage(QByteArray &Package, bool fcs)
{

#ifdef QT_DEBUG
    if (pushButton_Host->isChecked ()) {
        return ClientPaserPackage (Package, fcs);
    }
#endif
    return HostPaserPackage (Package, fcs);
}
