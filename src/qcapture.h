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
#include <QSqlTableModel>
#include <QMap>
#include "qhostpaser.h"

class QCapture : public QWidget
{
    Q_OBJECT
public:
    explicit QCapture(QWidget *parent = 0);
    ~QCapture();
    void Start();
    void Stop();
    void InsterOneItem(MeasureVal_t &val);
public slots:
    void DebugInfo ();
    void LogChanged(const QString &text);
    void LogTextEditAutoScroll();
    void AutoScroll();
    void ToggledCapture(bool toggled);
    void ClearData();
    void UpdateData(QList<MeasureVal_t> data);
    void SendCmdEraseAll();
    void SendCmdSetPara();
    void SendCmdSetTime();
    void WaitACK();
    void ReceiveACK();
    void DeviceIDChanged(const QString &text);
private:
    void PrepareWaitACK();
private:
    bool mAutoScroll;
    QTimer mTimer;
    int mWaitAckTime;
    QSqlTableModel *mModel;
    bool mIsStarted;
    QMap<QString, QHostPaser *> mListPort;
    QHostPaser *mPort;
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
        connect(pushButton, SIGNAL(toggled(bool)), pushButton_Clear, SLOT(setEnabled(bool)));
        pushButton_Clear->setEnabled (false);

        horizontalLayout->addWidget(pushButton_Clear);

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
        pushButton->setText(QApplication::translate("Capture", "StartRead", 0));
        pushButton_Clear->setText(QApplication::translate("Capture", "Clear", 0));
    } // retranslateUi

    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *label_DeviceID;
    QComboBox *comboBox;
    QComboBox *comboBox_DeviceID;
    QPushButton *pushButton;
    QPushButton *pushButton_setpara;
    QPushButton *pushButton_settime;
    QPushButton *pushButton_eraseall;
    QPushButton *pushButton_Clear;
    QSpacerItem *horizontalSpacer;
    QTreeView   *MainTreeView;
    QTreeView   *LogTreeView;
    QTextEdit   *LogTextEdit;
    QSplitter *splitter;

    QString LogText;
};

#endif // QCAPTURE_H
