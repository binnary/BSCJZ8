#ifndef QPROTOCOLBASE_H
#define QPROTOCOLBASE_H
#include <QObject>
#include <QEvent>
#include <map>
#include <QTime>
#include <QDate>
#include <QDateTime>
#include <QByteArray>
#include <QByteArrayData>
#include <QList>
#include <QDebug>

typedef struct{
    quint8 sec;
    quint8 min;
    quint8 hour;
    quint8 mday;
    quint8 mon;
//    quint8 wday;
    quint8 year;
}Time_t;

typedef struct{
    quint8  magic_code;
    Time_t cp_time;    //capture time
    quint16  pipe_type; //管道类型
    quint16  pipe_num; //管道编号
    float  CO;
    float  O2;
    float  abs_press;//绝压
    float  sfc_press;//表压
    float  flow;
    float  CO2;
    float  CH4;
    float  temperature;
}MeasureVal_t,*pMeasureVal_t;

typedef enum{
    M_CO=0,
    M_O2,
    M_PRESS_ABS,
    M_PRESS_SFC,
    M_FLOW,
    M_CO2,
    M_CH4,
    M_TEMP,
    MAX_MTYPE,
}eMeasureType;

typedef struct{
    float param_A;
    float param_B;
    float param_C;
    float param_D;
}FormulaParam_t;

typedef struct{
    quint8 m_interval;     //measure interval time,  minute
    quint8 s_interval;      //save data interval time, minute
    quint8 padding_1;
    quint8  device_id;
    FormulaParam_t fparam[MAX_MTYPE];
}Settings_t,*pSettings_t;

class QProtocol : public QObject
{
    Q_OBJECT
public:
    explicit QProtocol(QObject *parent = 0);
    enum PROTOCOL{
        STX=0x02,
    };
    enum {
        START=0x01,
        LEN,
        FCS,
    }FSM;
    enum CMD{
        CMD_ACK=0x06,
        CMD_ACK_ERR=0x07,
        CMD_UPLOAD=0x11,
        CMD_ERASE_ALL=0x12,
        CMD_SET_TIME=0x13,
        CMD_SET_PARAM=0x15,
    };
    void TestSelf();
    QByteArray makeCmdEraseAll(quint8 Addr=0);
    QByteArray makeCmdSetParam(quint8 Addr=0, QByteArray settings= QByteArray(""));
    QByteArray makeCmdSetTime(quint8 Addr=0, QDateTime datetime=QDateTime::currentDateTime());
    QByteArray makeCmdACK(quint8 Addr=0);
    QByteArray PaseraCmdACK(QByteArray data);
    QByteArray makeCmdNACK(quint8 Addr=0);
    QByteArray makeCmdUpload(quint8 Addr=0, QDate start=QDate::currentDate(), QDate end=QDate::currentDate());
    QList<MeasureVal_t> PaserRespCmdUpload(QByteArray data);
    quint8 makeFCS(QByteArray data);
    quint8 makeFCS(char data[]);
    QByteArray makeDate(QDate date);
    QString DumpArray(QByteArray data);
};
#endif // QPROTOCOLBASE_H
