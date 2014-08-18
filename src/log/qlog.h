#ifndef QLOG_H
#define QLOG_H
#include <QtCore>
#include <QString>
void MessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#endif // QLOG_H
