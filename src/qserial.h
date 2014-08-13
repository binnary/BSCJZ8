#ifndef QSERIAL_H
#define QSERIAL_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QVector>
#include <QMap>
#include <QList>

class QSerial
{
public:
    QSerial();
    QList<QSerialPortInfo>QGetPorts(){ return QSerialPortInfo::availablePorts ();}
};

#endif // QSERIAL_H
