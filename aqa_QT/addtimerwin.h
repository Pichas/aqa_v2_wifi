#ifndef ADDTIMERWIN_H
#define ADDTIMERWIN_H

#include <QDialog>

namespace Ui {
class addTimerWin;
}

class addTimerWin : public QDialog
{
    Q_OBJECT

public:
    explicit addTimerWin(QWidget *parent = nullptr);
    ~addTimerWin();

    void accept();

signals:
    void sendStringTimer(QString);

private:
    Ui::addTimerWin *ui;
};

#endif // ADDTIMERWIN_H
