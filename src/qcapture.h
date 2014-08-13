#ifndef QCAPTURE_H
#define QCAPTURE_H
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTableView>
#include <QWidget>
#include <QSplitter>
#include <QTableWidget>
#include <QTreeWidget>
#include <QTextEdit>
class QCapture : public QWidget
{
    Q_OBJECT
public:
    explicit QCapture(QWidget *parent = 0);

    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *comboBox;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer;
    QTreeWidget *treewidget;
    QTextEdit   *textedit;
    QSplitter *splitter;

    void DebugInfo ();
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

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        splitter = new QSplitter(Qt::Vertical, 0);
        treewidget = new QTreeWidget(splitter);
        treewidget->setObjectName(QStringLiteral("treeView_top"));
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
    } // retranslateUi
signals:

public slots:
};

#endif // QCAPTURE_H
