#-------------------------------------------------
#
# Project created by QtCreator 2014-08-04T10:36:04
#
#-------------------------------------------------

QT += core gui printsupport webkitwidgets
QT += serialport
QT += sql
QT += axcontainer
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
#contains (QWT_CONFIG, QwtOpenGL){
#      QT += opengl
#}else{
#    DEFINES +=QWT_NO_OPENGL
#}
#
#contains (QWT_CONFIG, QwtSvg){
#    QT += svg
#}else{
#    DEFINES +=QWT_NO_SVG
#}
#win32{
#   contains (QWT_CONFIG, QwtDll){
#     DEFINES += QT_DLL QWT_DLL
#  }
#}

#include (C:\Qt\qwt-6.1.0\qwt.prf)

SOURCES += \
    src/main.cpp\
    src/mainwindow.cpp \
    src/comcfg.cpp \
    src/qlogin.cpp \
    src/serial/qserial.cpp \
    src/protocol/qprotocolbase.cpp \
    src/qdispgraphic.cpp \
    src/deviceinfo.cpp \
    src/disassaydata.cpp \
    src/qcapture.cpp \
    src/style/norwegianwoodstyle.cpp \
    src/export/exportobject.cpp \
    src/print/qprintobject.cpp \
    src/qabout.cpp \
    src/dbopt/dbopt.cpp \
    src/log/qlog.cpp

HEADERS  += \
    src/mainwindow.h \
    src/comcfg.h \
    src/log/qlog.h \
    src/qlogin.h \
    src/serial/qserial.h \
    src/protocol/qprotocolbase.h \
    src/qdispgraphic.h \
    src/deviceinfo.h \
    src/disassaydata.h \
    src/qcapture.h \
    src/style/norwegianwoodstyle.h \
    src/export/exportobject.h \
    src/print/qprintobject.h \
    src/qabout.h \
    src/dbopt/dbopt.h

FORMS    += \
    ui/mainwindow.ui \
    ui/comcfg.ui \
    ui/qlogin.ui \
    ui/qdispgraphic.ui \
    ui/DeviceInfo.ui \
    ui/disassaydata.ui \
    ui/capture.ui \
    ui/AddNewDev.ui \
    ui/qabout.ui

RESOURCES += \
    res.qrc

OTHER_FILES +=
