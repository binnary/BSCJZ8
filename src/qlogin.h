#ifndef QLOGIN_H
#define QLOGIN_H

#include <QDialog>

namespace Ui
{
class QLogin;
}

class QLogin : public QDialog
{
    Q_OBJECT

public:
    explicit QLogin(QWidget *parent = 0);
    ~QLogin();
    bool is_login()
    {
        return mLogin;
    }
private:
    Ui::QLogin *ui;
    QString mUserName;
    QString mPassword;
    bool mLogin;
};

#endif // QLOGIN_H
