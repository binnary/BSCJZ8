#-------------------------------------------------
#
# Project created by QtCreator 2014-08-04T10:36:04
#
#-------------------------------------------------

QT += core gui printsupport
QT += sql
win32 {
 QT += webkitwidgets
 QT += serialport
 QT += axcontainer
}
QT += concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TRANSLATIONS += lang_CN.ts\
                lang_EN.ts

TARGET = Platform
TEMPLATE = app  qwt
PLUGINSPATH +=./
#QWT setting
QWT_ROOT="C:/Qt/qwt-6.1.0"
#LIBS += -lqwtd
include ($${QWT_ROOT}/qwtfunctions.pri)

SRCPATH = $${QWT_ROOT}/src
SRCPATH +=src
SRCPATH +=src/print
SRCPATH +=src/export
SRCPATH +=src/dbopt
SRCPATH +=src/log
SRCPATH +=src/serial
SRCPATH +=src/protocol
SRCPATH +=src/style

INCPATH += $${SRCPATH}
DEPENDPATH += $${SRCPATH}

contains (QWT_CONFIG, QwtFramework){
         LIBS += -F$${QWT_ROOT}/lib
}else{
    LIBS += -L$${QWT_ROOT}/lib
}
qwtAddLibrary(qwt)

SOURCES += \
    src/main.cpp\
    src/mainwindow.cpp \
    src/qlogin.cpp \
    src/qdispgraphic.cpp \
    src/deviceinfo.cpp \
    src/disassaydata.cpp \
    src/qcapture.cpp \
    src/style/norwegianwoodstyle.cpp \
    src/export/exportobject.cpp \
    src/print/qprintobject.cpp \
    src/qabout.cpp \
    src/dbopt/dbopt.cpp \
    src/log/qlog.cpp \
    src/protocol/qprotocol.cpp \
    src/serial/qhostpaser.cpp \
    src/setting.cpp \
    src/devparamconfig.cpp \
    src/qloading.cpp

HEADERS  += \
    src/mainwindow.h \
    src/log/qlog.h \
    src/qlogin.h \
    src/qdispgraphic.h \
    src/deviceinfo.h \
    src/disassaydata.h \
    src/qcapture.h \
    src/style/norwegianwoodstyle.h \
    src/export/exportobject.h \
    src/print/qprintobject.h \
    src/qabout.h \
    src/dbopt/dbopt.h \
    src/protocol/qprotocol.h \
    src/serial/qhostpaser.h \
    src/setting.h \
    src/devparamconfig.h \
    src/qloading.h

win32 {
    SOURCES +=\
    src/comcfg.cpp \
    src/serial/qserial.cpp
    HEADERS +=\
    src/serial/qserial.h \
    src/comcfg.h
}

FORMS    += \
    ui/mainwindow.ui \
    ui/comcfg.ui \
    ui/qlogin.ui \
    ui/qdispgraphic.ui \
    ui/DeviceInfo.ui \
    ui/disassaydata.ui \
    ui/capture.ui \
    ui/AddNewDev.ui \
    ui/qabout.ui \
    ui/devparamconfig.ui \
    ui/qloading.ui

RESOURCES += \
    res.qrc
TRANSLATIONS += \
    language/lang_CN.ts
