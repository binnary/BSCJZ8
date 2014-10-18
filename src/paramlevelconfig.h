#ifndef PARAMLEVELCONFIG_H
#define PARAMLEVELCONFIG_H

#include <QDialog>

namespace Ui
{
class ParamLevelConfig;
}

class ParamLevelConfig : public QDialog
{
    Q_OBJECT

public:
    explicit ParamLevelConfig(QWidget *parent = 0);
    ~ParamLevelConfig();
public slots:
    void UpdateValueChanged();
private:
    Ui::ParamLevelConfig *ui;
};

#endif // PARAMLEVELCONFIG_H
