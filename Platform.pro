#-------------------------------------------------
#
# Project created by QtCreator 2014-08-04T10:36:04
#
#-------------------------------------------------

QT += core gui printsupport
QT += serialport
QT += sql
QT +=axcontainer
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
INCPATH += $${QWT_ROOT}/src src
DEPENDPATH += $${QWT_ROOT}/src src

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
    src/qlog.cpp \
    src/qlogin.cpp \
    src/qserial.cpp \
    src/qprotocolbase.cpp \
    src/qdispgraphic.cpp \
    src/deviceinfo.cpp \
    src/disassaydata.cpp \
    src/exportobject.cpp \
    src/qcapture.cpp

HEADERS  += \
    src/mainwindow.h \
    src/comcfg.h \
    src/qlog.h \
    src/qlogin.h \
    src/qserial.h \
    src/qprotocolbase.h \
    src/Stdafx.h \
    src/qdispgraphic.h \
    src/deviceinfo.h \
    src/disassaydata.h \
    src/exportobject.h \
    src/qcapture.h

FORMS    += \
    ui/mainwindow.ui \
    ui/comcfg.ui \
    ui/qlogin.ui \
    ui/qdispgraphic.ui \
    ui/DeviceInfo.ui \
    ui/disassaydata.ui \
    ui/capture.ui

RESOURCES += \
    res.qrc
