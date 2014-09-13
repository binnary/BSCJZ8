#ifndef QCAPTURE_H
#define QCAPTURE_H
#include <QApplication>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QTreeView>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTableView>
#include <QWidget>
#include <QTimer>
#include <QSplitter>
#include <QTableWidget>
#include <QTreeWidget>
#include <QTextEdit>
#include <QSerialPort>
#include <QSqlTableModel>
#include <QMap>
#include <QMutex>
#include "setting.h"
#include "qprotocol.h"

class QCapture : public QWidget
{
    Q_OBJECT
public:
    enum {
        UPDATE_COMPORT_1000=1000,
        WAIT_ACK_TIMEOUT=3000,
        WAIT_UPLOAD_PACKAGE=4000,
    } TimerID;
    typedef enum {
        STATUS_NONE=0,
        STATUS_WAIT_SETPARAM_ACK,
        STATUS_WAIT_SETTIME_ACK,
        STATUS_WAIT_ERASEALL_ACK,
        STATUS_WAIT_ACK,
        STATUS_WAIT_PREPARE_UPLOAD,
        STATUS_WAIT_UPLOAD_QUERY,
    }Status_enum;
    explicit QCapture(QWidget *parent = 0);
    ~QCapture();
    void Start();
    void Stop();
    void InsterOneItem(quint8 DeviceID, MeasureVal_t &val);
    virtual void timerEvent(QTimerEvent *event);
    void ReceiveACK(quint32 Cmd, bool isReceiveAck);
public slots:
//    void DebugInfo ();
    void LogChanged(const QString &text);
    void LogTextEditAutoScroll();
    void AutoScroll();
    void ToggledCapture(bool toggled);
    void ToggledCmdUpload(bool toggled);
    void ClearData();
    void UpdateData(quint8 DeviceID, QList<MeasureVal_t> data);
    void SendCmdEraseAll();
    void SendCmdSetPara();
    void SendCmdSetTime();
    void SendCmdUpload();
//    void WaitACK();
    void CanReceiveData();
    void DeviceIDChanged(const QString &text);
private:
    bool HostPaserPackage (QByteArray &Package, bool fcs);
    bool ClientPaserPackage (QByteArray &Package, bool fcs);
   void RemoveTimer(int timeridIdKey);
   void InsertTimer(int timeridIdKey);
    void PrepareWaitACK(Status_enum status);
    quint8 CurrentDevID()
    {
        return Setting::GetInstance ().GetValue ("DeviceID").toInt ();
    }
    bool SelfTest()
    {
        if (Setting::GetInstance ().GetValue ("SelfTest").toInt () == 1){
            return true;
        }
        return false;
    }
    bool PaserPackage(QByteArray &Package, bool fcs=true);
    void SetStatus(Status_enum status){
        mMutex.lock ();
        mStatus = status;
        mMutex.unlock ();
    }
    Status_enum GetStatus(){return mStatus;}
private:
    Status_enum mStatus;
    QMutex mMutex;
    int mDataPackageCount;
    bool mAutoScroll;
    //Map<timer interval, timerid>
    QMap<int, int> mTimerIdMap;
    int mWaitAckTime;
    QSqlTableModel *mModel;
    QByteArray *mRecvData;
    QSerialPort *mPort;
    QProtocol mProtocol;
private: // ui
    void setupUi(QWidget *Capture)
    {
        if (Capture->objectName().isEmpty())
            Capture->setObjectName(QStringLiteral("Capture"));
        Capture->resize(722, 401);
        gridLayout = new QGridLayout(Capture);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

        label_DeviceID = new QLabel(Capture);
        label_DeviceID->setObjectName(QStringLiteral("label_DeviceID"));
        horizontalLayout->addWidget(label_DeviceID);

        comboBox_DeviceID = new QComboBox(Capture);
        comboBox_DeviceID->setObjectName(QStringLiteral("comboBox_DeviceID"));
        horizontalLayout->addWidget(comboBox_DeviceID);

        label = new QLabel(Capture);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        comboBox = new QComboBox(Capture);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        horizontalLayout->addWidget(comboBox);

        pushButton = new QPushButton(Capture);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setCheckable (true);
        horizontalLayout->addWidget(pushButton);

        pushButton_upload = new QPushButton(Capture);
        pushButton_upload->setObjectName(QStringLiteral("pushButton_setPara"));
        pushButton_upload->setText(QApplication::translate("Capture", "UpLoad", 0));
        pushButton_upload->setEnabled (false);
        pushButton_upload->setCheckable (true);
        connect(pushButton, SIGNAL(toggled(bool)), pushButton_upload, SLOT(setEnabled(bool)));
        horizontalLayout->addWidget(pushButton_upload);

        pushButton_setpara = new QPushButton(Capture);
        pushButton_setpara->setObjectName(QStringLiteral("pushButton_setPara"));
        pushButton_setpara->setText(QApplication::translate("Capture", "Set Param", 0));
        pushButton_setpara->setEnabled (false);
        connect(pushButton, SIGNAL(toggled(bool)), pushButton_setpara, SLOT(setEnabled(bool)));
        horizontalLayout->addWidget(pushButton_setpara);

        pushButton_settime = new QPushButton(Capture);
        pushButton_settime->setObjectName(QStringLiteral("pushButton_setPara"));
        pushButton_settime->setText(QApplication::translate("Capture", "Set Time", 0));
        pushButton_settime->setEnabled (false);
        connect(pushButton, SIGNAL(toggled(bool)), pushButton_settime, SLOT(setEnabled(bool)));
        horizontalLayout->addWidget(pushButton_settime);

        pushButton_eraseall = new QPushButton(Capture);
        pushButton_eraseall->setObjectName(QStringLiteral("pushButton_setPara"));
        pushButton_eraseall->setText(QApplication::translate("Capture", "Erase All", 0));
        pushButton_eraseall->setEnabled (false);
        connect(pushButton, SIGNAL(toggled(bool)), pushButton_eraseall, SLOT(setEnabled(bool)));
        horizontalLayout->addWidget(pushButton_eraseall);

        pushButton_Clear = new QPushButton(Capture);
        pushButton_Clear->setObjectName(QStringLiteral("pushButton_Clear"));
//        connect(pushButton, SIGNAL(toggled(bool)), pushButton_Clear, SLOT(setEnabled(bool)));
//        pushButton_Clear->setEnabled (false);

        horizontalLayout->addWidget(pushButton_Clear);
#ifdef QT_DEBUG
        pushButton_Host = new QPushButton(Capture);
        pushButton_Host->setObjectName(QStringLiteral("pushButton_Host"));
        pushButton_Host->setText(QApplication::translate("Capture", "Toggle Host", 0));
        pushButton_Host->setCheckable (true);
        horizontalLayout->addWidget(pushButton_Host);
#endif

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        splitter = new QSplitter(Qt::Vertical, 0);
        MainTreeView = new QTreeView(splitter);
        MainTreeView->setObjectName(QStringLiteral("treeView_main"));
        LogTextEdit = new QTextEdit(splitter);
        LogTextEdit->setObjectName(QStringLiteral("textedit"));
        LogTextEdit->setReadOnly (true);
        gridLayout->addWidget(splitter, 1, 0, 1, 1);

        retranslateUi(Capture);

        QMetaObject::connectSlotsByName(Capture);
    } // setupUi

    void retranslateUi(QWidget *Capture)
    {
        Capture->setWindowTitle(QApplication::translate("Capture", "Capture", 0));
        label->setText(QApplication::translate("Capture", "Port", 0));
        label_DeviceID->setText(QApplication::translate("Capture", "DeviceID", 0));
        pushButton->setText(QApplication::translate("Capture", "Open Device", 0));
        pushButton_Clear->setText(QApplication::translate("Capture", "Clear", 0));
    } // retranslateUi

    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *label_DeviceID;
    QComboBox *comboBox;
    QComboBox *comboBox_DeviceID;
    QPushButton *pushButton;
    QPushButton *pushButton_upload;
    QPushButton *pushButton_setpara;
    QPushButton *pushButton_settime;
    QPushButton *pushButton_eraseall;
    QPushButton *pushButton_Clear;
#ifdef QT_DEBUG
    QPushButton *pushButton_Host;
#endif
    QSpacerItem *horizontalSpacer;
    QTreeView   *MainTreeView;
    QTreeView   *LogTreeView;
    QTextEdit   *LogTextEdit;
    QSplitter *splitter;

    QString LogText;
};

#endif // QCAPTURE_H
