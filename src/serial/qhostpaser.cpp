#include "qhostpaser.h"

QHostPaser::QHostPaser(QSerialPortInfo info, QObject *parent) :
    QSerial(info,parent)
{
}

bool QHostPaser::PaserPackage(QByteArray &Package, bool fcs_state){
    QProtocol pro;
    qDebug() << "fcs=" << fcs_state << ":" << pro.DumpArray (Package);
    if (!fcs_state){
        Write(pro.makeCmdNACK (Package.at(2)));
        qWarning() << "Package check failed, skip this Package";
        return false;
    }
    Write(pro.makeCmdACK (Package.at(2)));
    if (QProtocol::CMD_UPLOAD == Package.at(3)){//CMD
        QList<MeasureVal_t> resp =
             mProtocol.PaserRespCmdUpload (Package.mid(4));
        emit PackagePaserDone(resp);
    }
    return true;
}
