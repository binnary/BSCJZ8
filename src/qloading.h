#ifndef QLOADING_H
#define QLOADING_H

#include <QTimer>
#include <QDialog>
#include <QAction>

namespace Ui
{
class QLoading;
}
#include <QDialog>
class QLoading : public QDialog
{
    Q_OBJECT

public:
    explicit QLoading(QDialog *parent = 0);
    ~QLoading();
    enum {
        DIRECTLOADING=0,
        LOADSETTING=1,
    } Status_e;
    int GetStatus()
    {
        return mStatus;
    }
public slots:
    void DevConfig();
    void DirectLogin();
    void timeout();
    void DeviceIDChanged(const QString &text);
private:
    void InitActions();
    Ui::QLoading *ui;
    QTimer mTimer;
    int mStatus;
    int mTimerCount;
};

#endif // QLOADING_H
