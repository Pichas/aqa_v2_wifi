#include "addtimerwin.h"
#include "ui_addtimerwin.h"

addTimerWin::addTimerWin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addTimerWin)
{
    ui->setupUi(this);
}

addTimerWin::~addTimerWin()
{
    delete ui;
}

void addTimerWin::accept()
{
    QString str = "A";

    str += ui->timeTimer->time().toString("HHmm");


    char daysOfWeek = 0;
    daysOfWeek |= ui->cb1->isChecked() << 0;
    daysOfWeek |= ui->cb2->isChecked() << 1;
    daysOfWeek |= ui->cb3->isChecked() << 2;
    daysOfWeek |= ui->cb4->isChecked() << 3;
    daysOfWeek |= ui->cb5->isChecked() << 4;
    daysOfWeek |= ui->cb6->isChecked() << 5;
    daysOfWeek |= ui->cb7->isChecked() << 6;

    str += daysOfWeek;

    str += QString::number(ui->cbAction->currentIndex() + 1);
    str += QString::number(ui->sbEffectNum->value());
    emit sendStringTimer(str);
    QDialog::accept();
}
