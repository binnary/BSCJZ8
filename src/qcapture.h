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

class QCapture : public QWidget
{
    Q_OBJECT
public:
    explicit QCapture(QWidget *parent = 0);
    ~QCapture();
    void Start();
    void Stop();
public slots:
    void DebugInfo ();
    void AutoScroll();
    void ToggledCapture();
    void ClearData();
private:
    bool mAutoScroll;
    QTimer timer;
    QSqlTableModel *mModel;
    bool mIsStarted;
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
        label = new QLabel(Capture);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        comboBox = new QComboBox(Capture);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        horizontalLayout->addWidget(comboBox);

        pushButton = new QPushButton(Capture);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout->addWidget(pushButton);

        pushButton_Clear = new QPushButton(Capture);
        pushButton_Clear->setObjectName(QStringLiteral("pushButton_Clear"));

        horizontalLayout->addWidget(pushButton_Clear);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        splitter = new QSplitter(Qt::Vertical, 0);
        treeView = new QTreeView(splitter);
        treeView->setObjectName(QStringLiteral("treeView_top"));
        textedit = new QTextEdit(splitter);
        textedit->setObjectName(QStringLiteral("textedit"));
        textedit->setReadOnly (true);
        gridLayout->addWidget(splitter, 1, 0, 1, 1);


        retranslateUi(Capture);

        QMetaObject::connectSlotsByName(Capture);
    } // setupUi

    void retranslateUi(QWidget *Capture)
    {
        Capture->setWindowTitle(QApplication::translate("Capture", "Form", 0));
        label->setText(QApplication::translate("Capture", "Port:", 0));
        pushButton->setText(QApplication::translate("Capture", "StartRead", 0));
        pushButton_Clear->setText(QApplication::translate("Capture", "Clear", 0));
    } // retranslateUi

    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *comboBox;
    QPushButton *pushButton;
    QPushButton *pushButton_Clear;
    QSpacerItem *horizontalSpacer;
    QTreeView   *treeView;
    QTextEdit   *textedit;
    QSplitter *splitter;
};

#endif // QCAPTURE_H
