#ifndef QABOUT_H
#define QABOUT_H

#include <QDialog>

namespace Ui {
class QAbout;
}

class QAbout : public QDialog
{
    Q_OBJECT

public:
    explicit QAbout(QWidget *parent = 0);
    ~QAbout();

private:
    Ui::QAbout *ui;
};

#endif // QABOUT_H
