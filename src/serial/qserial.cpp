#include "qserial.h"
#include "setting.h"
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>

QByteArray QSerial::ReadAll()
{
    QByteArray data("");
    mutex.lock ();
    if (!mPort->isOpen ()){
        goto ReadAllOut;
    }
    if (!mPort->bytesAvailable ()){
        msleep(100);
        goto ReadAllOut;
    }
    data = mPort->readAll ();
    if (data.size ()){
        QProtocol pro;
        qDebug() << "H<--D:"<< pro.DumpArray (data);
    }
ReadAllOut:
    mutex.unlock ();
    return data;
}
bool QSerial::Write(QByteArray data)
{
    QProtocol pro;
#ifdef DEBUG_SERIAL
    qDebug() << "Write:" << pro.DumpArray (data);
    return true;
#endif
   if (!mPort->isOpen ()) return false;
   mutex.lock ();
   qDebug() << "H-->D:"<< pro.DumpArray (data);
   mPort->write (data);
//   mPort->waitForBytesWritten (1000);
   mutex.unlock ();
   return true;
}
void QSerial::run()
{
   QByteArray RecvData("");
   QByteArray Package("");
   while(!isInterruptionRequested()){
#ifndef DEBUG_SERIAL
       QByteArray temp("");
       temp = ReadAll ();
       if (!temp.size ()){
           continue;
       }
       RecvData.append(temp);
       qDebug() <<"RecvData:" << mProtocol.DumpArray (RecvData);
#else
       msleep (100);
       QByteArray temp = Generate();
       RecvData.append(temp);
#endif

//       int mStatus = QProtocol::START;
//       qint32 PackageLen = 0;
//       while( i < RecvData.size ()){
//          switch(mStatus) {
//           case QProtocol::START:
//              if(QProtocol::STX == RecvData.at (i++)){
//                  mStatus = QProtocol::LEN;
//              }
//              break;
//           case QProtocol::LEN:
//              PackageLen = RecvData.at (i++);
//              if (RecvData.size ()-i >= PackageLen){
//                 goto PaserDone;
//              }
//              break;
//           case QProtocol::FCS:
//              Package = RecvData.mid(i, PackageLen+1);
//              i += PackageLen+1;
//              quint8 fcs = mProtocol.makeFCS(Package);
//              if (fcs ==  RecvData.at (i)){
//                  PaserPackage(Package);
//              }else{
//                  PaserPackage (Package, false);
//              }
//              break;
//         // default:
//         //     i++;
//      };
//     PaserDone:
//            RecvData = RecvData.mid(i);
//     }
// PACKAGE=STX+LEN+ADDR+CMD+DATA+FCS
// LEN = ADDR + CMD+DATA+FCS
//  FCS = STX+LEN+ADDR+CMD+DATA ^
       //i=Current STX
       int STX, PackageLen=0;
       for(STX=0; STX < RecvData.size()-1; ++STX){
           if (RecvData.at(STX) != QProtocol::STX){
               continue;
           }
           PackageLen = RecvData.at (STX+1);
           if (STX + PackageLen+1 >= RecvData.size()){
                //have not more data;
                RecvData = RecvData.mid(STX);
                break;
            }
//           qDebug() <<Q_FUNC_INFO << ":" <<__LINE__ << ":" << STX;
           //Package=LEN+ADDR+CMD+DATA
            Package = RecvData.mid(STX, PackageLen+1);
            quint8 fcs = mProtocol.makeFCS(Package);
//           qDebug() <<Q_FUNC_INFO << ":" <<__LINE__ << ":" << STX;
            if (fcs == RecvData.at(PackageLen+STX+1)){
                PaserPackage(Package);
                STX += PackageLen+1;
            }else{
                PaserPackage (Package, false);
            }
//           qDebug() <<Q_FUNC_INFO << ":" <<__LINE__ << ":" << STX;
//           i += RecvData.at(i+1)+1;
       }
//       qDebug() <<Q_FUNC_INFO << ":" <<__LINE__ << ":" << STX;
       if (STX==RecvData.size ()){
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
