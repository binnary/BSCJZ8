#include <QApplication>
#include <QDebug>
#include "mainwindow.h"
#include "norwegianwoodstyle.h"
#include <QStyleFactory>
#include <QMessageBox>
#include "qlog.h"
#include <dbopt/dbopt.h>
#include <setting.h>
#include <QTranslator>
#include <QTextCodec>
#include <QFont>
#include <setting.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator *translator = new QTranslator(NULL);
    QString language = Setting::GetInstance ().GetValue ("LANGUAGE").toString ();
    if (translator->load (language)){
        a.installTranslator (translator);
    }

    //NorwegianWood Windows WindowsXP Fusion WindowsVista
    QApplication::setStyle(QStyleFactory::create("Windows"));
    QApplication::setStyle(("Fusion"));
    QApplication::setStyle(QStyleFactory::create("WindowsXP"));
    QApplication::setStyle(QStyleFactory::create("WindowsVista"));
    QApplication::setStyle(QStyleFactory::create("Fusion"));
//    QApplication::setStyle(new NorwegianWoodStyle);
#ifndef QT_DEBUG
    qInstallMessageHandler(MessageOutput);
#endif
//    qInstallMessageHandler(MessageOutput);
    qDebug() << "Start---------------------------------------------------";
    Setting::GetInstance ();
    if (!db_open()) {
        return -1;
    }
    MainWindow w;
    w.show();

    return a.exec();
}
