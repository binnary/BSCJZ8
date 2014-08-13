#include "qcapture.h"
#include "ui_capture.h"
#include "QDebug"
#include <QAbstractTableModel>
#include <QSqlQuery>
#include <QTreeWidget>
#include <QDateTime>
#include <QSerialPortInfo>
QCapture::QCapture(QWidget *parent) :
    QWidget(parent)
{
    setupUi (this);
    QSqlQuery query;
    QString sql;
    QTextStream stream(&sql);
    stream << "PRAGMA table_info ('" << "AssayData" << "')";
    query.exec(sql);
    QStringList strList;
    while(query.next ()){
        strList << query.value ("name").toString ();
    }
    treewidget->setHeaderLabels (strList);
    QList<QSerialPortInfo> info = QSerialPortInfo::availablePorts ();
    QList<QSerialPortInfo>::iterator it;
    for(it= info.begin (); it!=info.end (); ++it){
         comboBox->addItem (((QSerialPortInfo)*it).portName ());
    }
    DebugInfo();
}

void QCapture::DebugInfo ()
{
    QDateTime time;
    time = QDateTime::currentDateTime ();
    QTreeWidgetItem *item = new QTreeWidgetItem(treewidget);
    item->setText (0, "1");
    item->setText (1, time.toString ("yyyy/M/d/mm:ss"));
    item->setText (2, "11");
    item->setText (3, "11");
    item->setText (4, "11");
    item->setText (5, "11");
    item->setText (6, "11");
//    QString sql;
//    QTextStream stream(&sql);
//    stream << "Insert into AssayData (DeviceId, AssayTime, PipeId, PipeType, Ch4) values(1,'"
//                 << time.toString ("yyyy/M/d/mm:ss") << "',0,00," << qrand()%1000 << ")";
//    query.exec(sql);
}
