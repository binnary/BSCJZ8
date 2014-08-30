#include "qprotocol.h"

QProtocol::QProtocol(QObject *parent) :
    QObject(parent)
{
}

QByteArray QProtocol::makeDate(QDate date)
{
    QByteArray ArrayDate("");
    ArrayDate.append((unsigned char)(date.year()-2000));
    ArrayDate.append((unsigned char)date.month());
    ArrayDate.append((unsigned char)date.day());
    return ArrayDate;
}

QByteArray QProtocol::makeCmdACK(unsigned char Addr)
{
//   STX, 0x03, 0x00, 0x6, FCS
    QByteArray Ack("");
    Ack.append(STX);
    Ack.append(0x03);/// setpage ack;
    Ack.append(Addr);
    Ack.append(CMD_ACK);
    Ack.append(makeFCS(Ack)) ;
    return Ack;
}
QByteArray QProtocol::makeCmdNACK(unsigned char Addr)
{
    QByteArray NAck("");
    NAck.append(STX);
    NAck.append(0x03);/// setpage ack;
    NAck.append(Addr);
    NAck.append((unsigned char)CMD_ACK_ERR);
    NAck.append(makeFCS(NAck)) ;
    return NAck;
}
//STX + LEN + ADDR + 0x11 + 参数 +  FCS
QByteArray QProtocol::makeCmdUpload(unsigned char Addr, QDate start, QDate end)
{
    QByteArray cmd("");
    QByteArray Astart= makeDate(start);
    QByteArray Aend= makeDate(end);
    cmd.append(STX);
    cmd.append(3+Astart.size()+Aend.size());
    cmd.append(Addr);
    cmd.append((unsigned char)CMD_UPLOAD);
    cmd.append(Astart);
    cmd.append(Aend);
    cmd.append(makeFCS(cmd));
    return cmd;
}
QByteArray QProtocol::makeCmdEraseAll(unsigned char Addr)
{
//    STX + LEN + ADDR + CMD_ERASE_ALL+ FCS
    QByteArray cmd("");
    cmd.append(STX);
    cmd.append(0x03);
    cmd.append(Addr);
    cmd.append((unsigned char)CMD_ERASE_ALL);
    cmd.append(makeFCS(cmd));
    return cmd;
}
QByteArray QProtocol::makeCmdSetParam(unsigned char Addr, QByteArray settings)
{
    QByteArray cmd("");
    cmd.append(STX);
    cmd.append(3+settings.size());
    cmd.append(Addr);
    cmd.append(CMD_SET_PARAM);
    cmd.append(settings);
    cmd.append(makeFCS(cmd));
    return settings;
}
QByteArray QProtocol::makeCmdSetTime(unsigned char Addr, QDateTime datetime)
{
   Time_t time_t;
   memset(&time_t, 0, sizeof(Time_t));
   QTime time = datetime.time();
   QDate date = datetime.date();
   time_t.hour = (unsigned char)time.hour();
   time_t.min  = (unsigned char)time.minute();
   time_t.sec  = (unsigned char)time.second();
   time_t.year = date.year()-2000;
   time_t.mday = date.day();
   time_t.mon = date.month();
   QByteArray cmd("");
   cmd.append(STX);
   cmd.append(sizeof(Time_t)+3);
   cmd.append(Addr);
   cmd.append((unsigned char)CMD_SET_TIME);
   cmd.append(QByteArray((char*)&time_t, sizeof(Time_t)));
   cmd.append(makeFCS(cmd));
   return cmd;
}

unsigned char QProtocol::makeFCS(QByteArray data)
{
    QByteArray::iterator it;
    unsigned char fcs = 0;
    for (it=data.begin(); it!=data.end(); it++){
            fcs ^= *it;
    }
    return fcs;
}
unsigned char QProtocol::makeFCS(char data[])
{
    unsigned char fcs = 0;
    for (unsigned int i=0; i < sizeof(data); ++i){
            fcs ^= data[i];
    }
    return fcs;
}

QList<MeasureVal_t> QProtocol::PaserRespCmdUpload(QByteArray data)
{
    QList<MeasureVal_t> listMeasure;
    for (int i =0; i < data.size()/(int)(sizeof(MeasureVal_t)); ++i){
           MeasureVal_t temp;
           memcpy(&temp, data.data(), sizeof(MeasureVal_t));
           listMeasure.push_back(temp);
    }
    return listMeasure;
}
QString QProtocol::DumpArray(QByteArray data){
   QByteArray::iterator it= data.begin();
   QString temp;
   for(; it!=data.end();++it){
      temp += QString("0x") + QString::number((unsigned char)*it,16) +QString(" ");
   }
   return temp;
}
void QProtocol::TestSelf()
{
    qDebug() << DumpArray(makeCmdACK(2));
    qDebug() << DumpArray(makeCmdNACK(2));
    qDebug() << DumpArray(makeCmdSetTime());
}
