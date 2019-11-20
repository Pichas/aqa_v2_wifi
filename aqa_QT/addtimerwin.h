#ifndef ADDTIMERWIN_H
#define ADDTIMERWIN_H

#include <QDialog>
#include "devicesm.h"

namespace Ui {
class addTimerWin;
}

class addTimerWin : public QDialog
{
    Q_OBJECT

public:
    explicit addTimerWin(DeviceSM* device, int num = -1, QWidget *parent = nullptr);
    ~addTimerWin();

    void accept();

signals:
    void sendStringTimer(QTime, uint8_t, QString, QString);

private:
    Ui::addTimerWin *ui;
    DeviceSM *m_dev;
    int m_num;
};

#endif // ADDTIMERWIN_H
