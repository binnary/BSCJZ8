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
    unsigned char sec;
    unsigned char min;
    unsigned char hour;
    unsigned char mday;
    unsigned char mon;
//    unsigned char wday;
    unsigned char year;
}Time_t;

typedef struct{
    unsigned char   magic_code;
    Time_t cp_time;    //capture time
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
    M_PRESS,
    M_FLOW,
    M_CO2,
    M_CH4,
    M_TEMP,
    MAX_MTYPE,
}eMeasureType;

typedef struct{
    float param_A;
    float param_B;
}FormulaParam_t;

typedef struct{
    unsigned char m_interval;     //measure interval time,  minute
    unsigned char s_interval;      //save data interval time, minute
    unsigned char padding_1;
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
    enum CMD{
        CMD_ACK=0x06,
        CMD_ACK_ERR=0x07,
        CMD_UPLOAD=0x11,
        CMD_ERASE_ALL=0x12,
        CMD_SET_TIME=0x13,
        CMD_SET_PARAM=0x15,
    };
    void TestSelf();
    QByteArray makeCmdEraseAll(unsigned char Addr=0);
    QByteArray makeCmdSetParam(unsigned char Addr=0, QByteArray settings= QByteArray(""));
    QByteArray makeCmdSetTime(unsigned char Addr=0, QDateTime datetime=QDateTime::currentDateTime());
    QByteArray makeCmdACK(unsigned char Addr=0);
    QByteArray PaseraCmdACK(QByteArray data);
    QByteArray makeCmdNACK(unsigned char Addr=0);
    QByteArray makeCmdUpload(unsigned char Addr=0, QDate start=QDate::currentDate(), QDate end=QDate::currentDate());
    QList<MeasureVal_t> PaserRespCmdUpload(QByteArray data);
    unsigned char makeFCS(QByteArray data);
    unsigned char makeFCS(char data[]);
    QByteArray makeDate(QDate date);
    QString DumpArray(QByteArray data);
};
#endif // QPROTOCOLBASE_H
