#include "qserial.h"
#include "setting.h"
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>

QByteArray QSerial::ReadAll()
{
    QByteArray data("");
    if (!mPort->isOpen ()){
        return data;
    }
    if (!mPort->bytesAvailable ()){
        msleep(100);
        return data;
    }
    data = mPort->readAll ();
    if (data.size ()){
        QProtocol pro;
        qDebug() << "H<--D:"<< pro.DumpArray (data);
    }
    return data;
}
QByteArray QSerial::Write(QByteArray data)
{
    QProtocol pro;
#ifdef DEBUG_SERIAL
    qDebug() << "Write:" << pro.DumpArray (data);
    return data;
#endif
   if (!mPort->isOpen ()) return data;
   mutex.lock ();
   qDebug() << "H-->D:"<< pro.DumpArray (data);
   mPort->write (data);
   mutex.unlock ();
   return data;
}
void QSerial::run()
{
   QByteArray RecvData("");
   QByteArray Package("");
   while(!isInterruptionRequested()){
#ifndef DEBUG_SERIAL
       RecvData.append(ReadAll ());
#else
       msleep (100);
       QByteArray temp = Generate();
       RecvData.append(temp);
#endif
       int i = 0;
       for(; i < RecvData.size(); ++i){
           if (RecvData.at(i) != QProtocol::STX){
               continue;
           }
           if (RecvData.at(i+1)+1 >= RecvData.size()-i){
                //have not more data;
                RecvData = RecvData.mid(i);
                break;
            }
            Package = RecvData.mid(i, RecvData.at(i+1)+1);
            quint8 fcs = mProtocol.makeFCS(Package);
            if (fcs == RecvData.at(RecvData.at(i+1)+1+i)){
                PaserPackage(Package);
            }else{
                PaserPackage (Package, false);
            }
            i += RecvData.at(i+1)+1;
       }
       if (i==RecvData.size ()){
           RecvData.clear ();
       }
   }
   qDebug() << "Thread done";
   return;
}
QByteArray QSerial::Generate()
{
    QByteArray temp("");
//        "0x2 0x3 0x2 0x6 0x5 "
    static bool isFirst= true;
    if (isFirst){
        temp.append(0x02);
        temp.append(0x03);
        temp.append(0x02);
        temp.append(0x06);
        temp.append(0x05);
//      "0x2 0x3 0x2 0x7 0x4 "
//      "0x2 0xa 0x0 0x13 0x29 0x24 0x11 0x1c 0x8 0x0 0xe 0x1d "
        temp.append(0x02);
        temp.append(0x0a);
        temp.append('\0');
        temp.append(0x13);
        isFirst = false;
    }else{
        temp.append(0x29);
        temp.append(0x24);
        temp.append(0x11);
        temp.append(0x1c);
        temp.append(0x08);
        temp.append('\0');
        temp.append(0x0e);
        temp.append(0x1d);
        isFirst =true;
    }
    return temp;
}
bool QSerial::InitPort()
{
   //temp["Baud"] = QSerialPort::Baud9600;
   //temp["Data"] = QSerialPort::Data8;
   //temp["Parity"] = QSerialPort::NoParity;
   //temp["Stop"] = QSerialPort::OneStop;
   //temp["Flow"] = QSerialPort::NoFlowControl;

   Setting &set = Setting::GetInstance ();
   mPort->setBaudRate(set.GetValue ("COM/Baud").toInt(),QSerialPort::AllDirections);
   mPort->setDataBits((QSerialPort::DataBits)set.GetValue ("COM/Data").toInt());
   mPort->setParity((QSerialPort::Parity)set.GetValue ("COM/Parity").toInt ());
   mPort->setStopBits((QSerialPort::StopBits)set.GetValue ("COM/Stop").toInt ());
   mPort->setFlowControl((QSerialPort::FlowControl)set.GetValue ("COM/Flow").toInt ());
   if (!mPort->open (QSerialPort::ReadWrite)){
       qDebug() << "Open serial port failed";
       return false;
   }
   return true;
}
bool QSerial::PaserPackage(QByteArray &Package, bool fcs_state)
{
    //default dump package
    qDebug() << "fcs:" << fcs_state << "," << mProtocol.DumpArray(Package);
    return true;
}
