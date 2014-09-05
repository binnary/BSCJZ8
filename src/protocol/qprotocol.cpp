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

QByteArray QProtocol::makeCmdACK(quint8 Addr)
{
//   STX, 0x03, 0x00, 0x6, FCS
    return makePackage (Addr, CMD_ACK);
   // QByteArray Ack("");
   // Ack.append(STX);
   // Ack.append(0x03);/// setpage ack;
   // Ack.append(Addr);
   // Ack.append(CMD_ACK);
   // Ack.append(makeFCS(Ack)) ;
   // return Ack;
}
QByteArray QProtocol::makeCmdNACK(quint8 Addr)
{
    return makePackage (Addr, CMD_ACK_ERR);
    //QByteArray NAck("");
    //NAck.append(STX);
    //NAck.append(0x03);/// setpage ack;
    //NAck.append(Addr);
    //NAck.append((quint8)CMD_ACK_ERR);
    //NAck.append(makeFCS(NAck)) ;
    //return NAck;
}
//STX + LEN + ADDR + 0x11 + 参数 +  FCS
QByteArray QProtocol::makeCmdUpload(quint8 Addr, QDate start, QDate end)
{
//    QByteArray ("");
    //QByteArray Astart= makeDate(start);
    //QByteArray Aend= makeDate(end);
    return makePackage(Addr, CMD_UPLOAD, makeDate(start)+makeDate(end));
    //cmd.append(STX);
    //cmd.append(3+Astart.size()+Aend.size());
    //cmd.append(Addr);
    //cmd.append((quint8)CMD_UPLOAD);
    //cmd.append(Astart);
    //cmd.append(Aend);
    //cmd.append(makeFCS(cmd));
    //return cmd;
}
QByteArray QProtocol::makeCmdEraseAll(quint8 Addr)
{
//    STX + LEN + ADDR + CMD_ERASE_ALL+ FCS
    return makePackage(Addr, CMD_ERASE_ALL);//, QByteArray(""));
    //QByteArray cmd("");
    //cmd.append(STX);
    //cmd.append(0x03);
    //cmd.append(Addr);
    //cmd.append((quint8)CMD_ERASE_ALL);
    //cmd.append(makeFCS(cmd));
    //return cmd;
}
QByteArray QProtocol::makeCmdSetParam(quint8 Addr, QByteArray settings)
{
    return makePackage(Addr, CMD_SET_PARAM, settings);
   // QByteArray cmd("");
   // cmd.append(STX);
   // cmd.append(3+settings.size());
   // cmd.append(Addr);
   // cmd.append(CMD_SET_PARAM);
   // cmd.append(settings);
   // cmd.append(makeFCS(cmd));
   // return cmd;
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
  // QByteArray cmd("");
  // cmd.append(STX);
  // cmd.append(sizeof(Time_t)+3);
  // cmd.append(Addr);
  // cmd.append((quint8)CMD_SET_TIME);
  // cmd.append(QByteArray((char*)&time_t, sizeof(Time_t)));
  // cmd.append(makeFCS(cmd));
  // return cmd;
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
QByteArray QProtocol::makeUploadResp(quint8 PackageLen)
{
    QByteArray data;
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
