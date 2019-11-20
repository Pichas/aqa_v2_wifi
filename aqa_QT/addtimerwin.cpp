#include "addtimerwin.h"
#include "ui_addtimerwin.h"

addTimerWin::addTimerWin(DeviceSM *device, int num, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addTimerWin)
{
    ui->setupUi(this);
    m_dev = device;
    m_num = num;

    if (num >= 0) this->setWindowTitle("Изменить таймер");


    ui->cbAction->addItems(m_dev->actionsName());
    ui->cbEff->addItems(m_dev->effectsName());

    connect(ui->cbWD, &QCheckBox::clicked, [&](bool b){
        ui->cb1->setChecked(b);
        ui->cb2->setChecked(b);
        ui->cb3->setChecked(b);
        ui->cb4->setChecked(b);
        ui->cb5->setChecked(b);
    });

    connect(ui->cbWE, &QCheckBox::clicked, [&](bool b){
        ui->cb6->setChecked(b);
        ui->cb7->setChecked(b);
    });

    connect(ui->cbAll, &QCheckBox::clicked, [&](bool b){
        ui->cb1->setChecked(b);
        ui->cb2->setChecked(b);
        ui->cb3->setChecked(b);
        ui->cb4->setChecked(b);
        ui->cb5->setChecked(b);
        ui->cb6->setChecked(b);
        ui->cb7->setChecked(b);
        ui->cbWD->setChecked(b);
        ui->cbWE->setChecked(b);
    });
}

addTimerWin::~addTimerWin()
{
    delete ui;
}

void addTimerWin::accept()
{
    QTime time = ui->timeTimer->time();

    uint8_t daysOfWeek = 0;
    daysOfWeek |= ui->cb1->isChecked() << 0;
    daysOfWeek |= ui->cb2->isChecked() << 1;
    daysOfWeek |= ui->cb3->isChecked() << 2;
    daysOfWeek |= ui->cb4->isChecked() << 3;
    daysOfWeek |= ui->cb5->isChecked() << 4;
    daysOfWeek |= ui->cb6->isChecked() << 5;
    daysOfWeek |= ui->cb7->isChecked() << 6;
    if (daysOfWeek == 0) return;

    if (m_num >= 0)
        m_dev->editTimer(time, daysOfWeek, ui->cbAction->currentText(), ui->cbEff->currentText(), m_num);
    else
        m_dev->addTimer(time, daysOfWeek, ui->cbAction->currentText(), ui->cbEff->currentText());

    QDialog::accept();
}
