#include <QApplication>
#include <QDebug>
#include <QStyleFactory>
#include <QMessageBox>
#include <QTranslator>
#include <QTextCodec>
#include <QFont>
#include "dbopt.h"
#include "qlog.h"
#include "setting.h"
#include "mainwindow.h"
#include "norwegianwoodstyle.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator *translator = new QTranslator(NULL);
    QString language = Setting::GetInstance ().GetValue ("LANGUAGE").toString ();
    if (translator->load (language)) {
        a.installTranslator (translator);
    }

    QIcon icon;
    icon.addFile(QStringLiteral(":/images/preferences-system.png"), QSize(), QIcon::Normal, QIcon::Off);
    a.setWindowIcon (icon);
    //NorwegianWood Windows WindowsXP Fusion WindowsVista
//    QApplication::setStyle(QStyleFactory::create("Windows"));
//    QApplication::setStyle(("Fusion"));
//    QApplication::setStyle(QStyleFactory::create("WindowsXP"));
//    QApplication::setStyle(QStyleFactory::create("WindowsVista"));
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
    QFont font;
    font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
    font.setPointSize(14);
    a.setFont(font);

    MainWindow w;
    w.show();

    return a.exec();
}
