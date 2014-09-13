#include "qprotocol.h"

QProtocol::QProtocol(QObject *parent) :
    QObject(parent),
    mIsDebug(false)
{
}

QByteArray QProtocol::makeDate(QDate date)
{
    QByteArray ArrayDate("");
    ArrayDate.append((quint8)(date.year()-2000));
    ArrayDate.append((quint8)date.month());
    ArrayDate.append((quint8)date.day());
    return ArrayDate;
}

QByteArray QProtocol::makeCmdACK(quint8 Cmd, quint8 Addr)
{
//   STX, 0x03, 0x00, 0x6,data, FCS
    char cdata = 0;
    QByteArray data((char*)&Cmd, 1);
    data.append (cdata);
    return makePackage (Addr, CMD_ACK, data);
}
QByteArray QProtocol::makeCmdNACK(quint8 Cmd, quint8 Addr)
{
    QByteArray data("");
    data.append (Cmd);
    data.append (1);
    return makePackage (Addr, CMD_ACK_ERR, data);
}
//STX + LEN + ADDR + 0x11 + 参数 +  FCS
QByteArray QProtocol::makeCmdUpload(quint8 Addr, QDate start, QDate end)
{
    return makePackage(Addr, CMD_UPLOAD, makeDate(start)+makeDate(end));
}
QByteArray QProtocol::makeCmdUploadQuery(quint8 Addr, QDate start, QDate end)
{
    return makePackage(Addr, CMD_UPLOAD_QUERY, makeDate(start)+makeDate(end));
}
QByteArray QProtocol::makeCmdEraseAll(quint8 Addr)
{
//    STX + LEN + ADDR + CMD_ERASE_ALL+ FCS
    return makePackage(Addr, CMD_ERASE_ALL);//, QByteArray(""));
}
QByteArray QProtocol::makeCmdSetParam(quint8 Addr, QByteArray settings)
{
    return makePackage(Addr, CMD_SET_PARAM, settings);
}
QByteArray QProtocol::makePackage(quint8 Addr, quint8 Cmd, QByteArray data)
{
   QByteArray cmd("");
   cmd.append(STX);
   cmd.append(3+data.size());
   cmd.append(Addr);
   cmd.append(Cmd);
   cmd.append(data);
   cmd.append(makeFCS(cmd));
   return cmd;
}
QByteArray QProtocol::makeCmdSetTime(quint8 Addr, QDateTime datetime)
{
   Time_t time_t;
   memset(&time_t, 0, sizeof(Time_t));
   QTime time = datetime.time();
   QDate date = datetime.date();
   time_t.hour = (quint8)time.hour();
   time_t.min  = (quint8)time.minute();
   time_t.sec  = (quint8)time.second();
   time_t.year = date.year()-2000;
   time_t.mday = date.day();
   time_t.mon = date.month();
   return makePackage (Addr, CMD_SET_TIME, QByteArray((char*)&time_t, sizeof(Time_t)));
}

quint8 QProtocol::makeFCS(QByteArray data)
{
    QByteArray::iterator it;
    quint8 fcs = 0;
    for (it=data.begin(); it!=data.end(); it++){
            fcs ^= (quint8)*it;
    }
    return fcs;
}
quint8 QProtocol::makeFCS(char data[])
{
    quint8 fcs = 0;
    for (unsigned int i=0; i < sizeof(data); ++i){
            fcs ^= data[i];
    }
    return fcs;
}
QByteArray QProtocol::makeUploadResp(quint8 PackageLen, quint32 PackageNumber)
{
    QByteArray data((char*)&PackageNumber, sizeof(PackageNumber));
    for (quint8 i=0; i < PackageLen; ++i){
        MeasureVal_t temp;
        memset(&temp, 0, sizeof(MeasureVal_t));
        temp.magic_code = 0x55;
        QTime time = QTime::currentTime ();
        QDate date = QDate::currentDate ();
        temp.cp_time.hour = (quint8)time.hour();
        temp.cp_time.min  = (quint8)time.minute();
        temp.cp_time.sec  = (quint8)time.second();
        temp.cp_time.year = date.year()-2000;
        temp.cp_time.mday = date.day();
        temp.cp_time.mon = date.month();
        temp.pipe_num = (float)(qrand()%10);
        temp.pipe_type = (float)(qrand()%20);
        temp.CH4 = (float)(qrand()%30);
        temp.CO = (float)(qrand()%40);
        temp.O2 = (float)(qrand()%50);
        temp.abs_press = (float)(qrand()%60);
        temp.sfc_press = (float)(qrand()%70);
        temp.flow = (float)(qrand()%80);
        temp.temperature = (float)(qrand()%90);
        data.append (QByteArray((char*)&temp, sizeof(MeasureVal_t)));
    }
    return data;
}

QList<MeasureVal_t> QProtocol::PaserRespCmdUpload(QByteArray data)
{
    QList<MeasureVal_t> listMeasure;
    const char* chdata = data.data ();
    for (int i =0; i < data.size()/(int)(sizeof(MeasureVal_t)); ++i){
           MeasureVal_t temp;
           memcpy(&temp, chdata+sizeof(MeasureVal_t)*i, sizeof(MeasureVal_t));
           listMeasure.push_back(temp);
    }
    return listMeasure;
}
QString QProtocol::DumpArray(QByteArray data){
   QByteArray::iterator it= data.begin();
   QString temp;
   if(!data.size ()){
       return temp;
   }
   for(; it!=data.end();++it){
      temp += QString("0x") + QString::number((quint8)*it,16) +QString(" ");
   }
   return temp;
}
void QProtocol::TestSelf()
{
    qDebug() << DumpArray(makeCmdACK(2));
    qDebug() << DumpArray(makeCmdNACK(2));
    qDebug() << DumpArray(makeCmdSetTime());
}
