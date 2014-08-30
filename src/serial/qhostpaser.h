#ifndef QHOSTPASER_H
#define QHOSTPASER_H

#include "qserial.h"
#include <QSerialPortInfo>
class QHostPaser : public QSerial
{
    Q_OBJECT
public:
    explicit QHostPaser(QSerialPortInfo info, QObject *parent = 0);
    virtual bool PaserPackage(QByteArray &Package, bool fcs_state=true);
signals:
    void PackagePaserDone(QList<MeasureVal_t>);
};

#endif // QHOSTPASER_H
