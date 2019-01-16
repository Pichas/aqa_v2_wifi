#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    device.reset(new driver(this));
    connect(device.data(), &driver::sendMessage, ui->statusBar, &QStatusBar::showMessage);
    connect(device.data(), &driver::sendEffectChanged, this, &MainWindow::refreshActions);

    connect(ui->pbExport,  &QPushButton::clicked, this, &MainWindow::exportTimers);
    connect(ui->pbImport,  &QPushButton::clicked, this, &MainWindow::importTimers);



    connect(ui->actIPPort, &QAction::triggered, this, &MainWindow::setIPPort);
    connect(ui->actConnect, &QAction::triggered, [this]{
        device->conn();
        this->loadStatus();        //обновить интерфейс
    });



    connect(ui->actGetTime, &QAction::triggered, [&]{device->getTimeFromDevice();});
    connect(ui->actSyncTime, &QAction::triggered, [&]{device->syncTimeToDevice();});

    connect(ui->actRelay0, &QAction::triggered, [&](int ch){device->setRelayStatus(0,ch);});
    connect(ui->actRelay1, &QAction::triggered, [&](int ch){device->setRelayStatus(1,ch);});

    connect(ui->cbEnTimers, &QCheckBox::clicked, [&](int ch){device->setEnTimers(ch);});

    //effects
    connect(ui->actLedOff,      &QAction::triggered, [&]{device->setEffect(0);});
    connect(ui->actSunrise,     &QAction::triggered, [&]{device->setEffect(1);});
    connect(ui->actSunset,      &QAction::triggered, [&]{device->setEffect(2);});
    connect(ui->actLedOn,       &QAction::triggered, [&]{device->setEffect(3);});

    connect(ui->actSetUserColor,&QAction::triggered, [&]{
            static QColor color = Qt::white;
            color = QColorDialog::getColor(color, this, "Выбор цвета");
            device->setEffect(4);
            device->setUserColor(0, color.toRgb().red(), color.toRgb().green(), color.toRgb().blue());
            ui->menu_7->setStyleSheet("QMenu::item { background-color: " + color.name() + "; }");
        });


    connect(ui->actCustomMode,  &QAction::triggered, [&]{device->setEffect(5); this->customMode();});


    //timers
    connect(ui->pbAddTimer, &QPushButton::clicked, [&]{
        QScopedPointer<addTimerWin> win(new addTimerWin());
        connect(win.data(), &addTimerWin::sendStringTimer, device.data(), &driver::addTimer);
        win->exec();
        device->loadTimers(ui->tvAllTimers); //обновить таймеры
    });

    connect(ui->pbDelTimer, &QPushButton::clicked, [&]{
        if (ui->tvAllTimers->selectionModel() && ui->tvAllTimers->selectionModel()->hasSelection())
            device->removeTimer(ui->tvAllTimers->selectionModel()->selectedRows().at(0).data().toInt());
        device->loadTimers(ui->tvAllTimers); //обновить таймеры
    });


    connect(ui->actRefresh,  &QAction::triggered, this, &MainWindow::loadStatus);


    ui->actRefresh->setShortcut(Qt::Key_F5);
    ui->actExit->setShortcut(Qt::Key_F12);






    //matrix
    connect(ui->pbMakeMatrix, &QPushButton::clicked, this, &MainWindow::makeMatrix);
    connect(ui->pbDelMatrix, &QPushButton::clicked, this, &MainWindow::delMatrix);

    ui->sbCount->setValue(SETTING_INI->value("LEDS/count",0).toInt());
    ui->sbStart->setValue(SETTING_INI->value("LEDS/start",0).toInt());
    ui->sbElementsInRow->setValue(SETTING_INI->value("LEDS/columns",0).toInt());

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setIPPort()
{
    QString IP = SETTING_INI->value("CONNECT/IP", "127.0.0.1").toString();
    int Port = SETTING_INI->value("CONNECT/Port", 5555).toInt();

    bool ok;
    QString text = QInputDialog::getText(this, "Указать подключение",
                                         tr("IP:Port"), QLineEdit::Normal,
                                         IP + ":" + QString::number(Port), &ok);
    if (ok && !text.isEmpty()){
        SETTING_INI->setValue("CONNECT/IP", text.split(":").at(0));
        SETTING_INI->setValue("CONNECT/PORT", text.split(":").at(1));
    }
}





void MainWindow::customMode()
{
    if (device->getEffectIndex() != 5) return; //выйти, если не тот эффект
    ui->pbarUploading->setMaximum(allLeds.count() - 1);

    for (int i = 0 ; i < allLeds.count() ; i++){
        ui->pbarUploading->setValue(i);
        QColor c = allLeds.value(allLeds.keys().at(i));
        QThread::msleep(150);
        device->setUserColor(allLeds.keys().at(i)->text().toInt(), c.red(), c.green(),c.blue());
    }
}

void MainWindow::loadStatus()
{
    device->getStatus();
    QTimer::singleShot(50, this, &MainWindow::refreshActions);        // обновить элементы отображения
    QTimer::singleShot(50, [&]{device->loadTimers(ui->tvAllTimers);});        // обновить список таймеров
}

void MainWindow::refreshActions()
{    ui->cbEnTimers->setChecked(device->isEnTimers());
     ui->actRelay0->setChecked(device->isRelay(0));
     ui->actRelay1->setChecked(device->isRelay(1));

     ui->actLedOff->setChecked(device->getEffectIndex() == 0);
     ui->actSunrise->setChecked(device->getEffectIndex() == 1);
     ui->actSunset->setChecked(device->getEffectIndex() == 2);
     ui->actLedOn->setChecked(device->getEffectIndex() == 3);
     ui->actSetUserColor->setChecked(device->getEffectIndex() == 4);
     ui->actCustomMode->setChecked(device->getEffectIndex() == 5);
}

void MainWindow::exportTimers()
{
    if (!ui->tvAllTimers->model()) return;
    timersModelTable* model = qobject_cast<timersModelTable*>(ui->tvAllTimers->model());


    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Открыть файл с таймерами"),
                                                    QDir::homePath(),
                                                    tr("Файл таймеров (*.tim);;Все файлы (*)"));

    if (!fileName.isEmpty()){
        QFile file(fileName);
        if(file.open(QIODevice::WriteOnly)){
            QDataStream stream(&file);
            stream.setVersion(QDataStream::Qt_4_2);

            stream << QString("Аквариумные таймеры");
            stream << model->rowCount(); //записать количество записей

            for (int i = 0 ; i < model->rowCount() ; i++){
                QString row = "A";

                row += model->index(i,1).data().toTime().toString("HHmm");

                QString wd = model->index(i,2).data().toString();
                char daysOfWeek = 0;
                daysOfWeek |= wd.contains("ПН") << 0;
                daysOfWeek |= wd.contains("ВТ") << 1;
                daysOfWeek |= wd.contains("СР") << 2;
                daysOfWeek |= wd.contains("ЧТ") << 3;
                daysOfWeek |= wd.contains("ПТ") << 4;
                daysOfWeek |= wd.contains("СБ") << 5;
                daysOfWeek |= wd.contains("ВС") << 6;

                row += daysOfWeek;

                QString action = model->index(i,3).data().toString();;

                if (action == "ВЫключить реле 0") row += '1';
                if (action == "Включить реле 0") row += '2';
                if (action == "ВЫключить реле 1") row += '3';
                if (action == "Включить реле 1") row += '4';
                if (action == "Переключить эффект") row += '5';

                row += model->index(i,4).data().toString();
                stream << row;
            }

            if(stream.status() != QDataStream::Ok){
                QMessageBox::warning(nullptr, "Экспорт таймеров", "Ошибка экспорта");
            }else{
                QMessageBox::information(nullptr, "Экспорт таймеров", "Экспорт завершен успешно");
            }
        }
        file.close();
    }

}

void MainWindow::importTimers()
{
    QString loadedTimer;


    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Открыть файл с таймерами"),
                                QDir::homePath(),
                                tr("Файл таймеров (*.tim);;Все файлы (*)"));
    if (!fileName.isEmpty()){
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly)){

            QDataStream stream(&file);
            stream.setVersion (QDataStream::Qt_4_2) ;

            QString check;
            stream >> check;
            if (check != "Аквариумные таймеры") {
                QMessageBox::warning(nullptr, "Импорт таймеров", "Это не файл таймеров");
                return;
            }


            int dataCount;
            stream >> dataCount;
            qDebug() << dataCount;

            for (int i = 0; i < dataCount ; i++){
                stream >> loadedTimer;
                device->addTimer(loadedTimer);
                QThread::msleep(150);
                QCoreApplication::processEvents();
            }

            if(stream.status() != QDataStream::Ok){
                QMessageBox::warning(nullptr, "Импорт таймеров", "Ошибка импорта");
            }
            file.close();
        }
        this->loadStatus();
    }
}






void MainWindow::makeMatrix()
{
    delMatrix();//удалить старые
    this->resize(662, 437);

    const static QSize btnSize = QSize(20, 20);
    int row = 0;
    int column = 0;

    for (int i = ui->sbStart->value() ; i < ui->sbCount->value() ; i++){
        QPushButton* b = new QPushButton(QString::number(i), this);
        allLeds.insert(b, QColor(Qt::black));
        b->setFixedSize(btnSize);

        connect(b, &QPushButton::clicked, this, &MainWindow::pickColor);

        ui->ledGrid->addWidget(b, row, column++);
        if (column >= ui->sbElementsInRow->value()){
            column = 0;
            row++;
        }
    }
    ui->ledGrid->setSpacing(0);

    SETTING_INI->setValue("LEDS/count",ui->sbCount->value());
    SETTING_INI->setValue("LEDS/start",ui->sbStart->value());
    SETTING_INI->setValue("LEDS/columns",ui->sbElementsInRow->value());

}

void MainWindow::delMatrix()
{
    while(allLeds.count() > 0){
        allLeds.firstKey()->deleteLater();
        allLeds.remove(allLeds.firstKey());
    }
}


void MainWindow::pickColor()
{
    QPushButton* b = qobject_cast<QPushButton*> (QObject::sender());

    const QColor color = QColorDialog::getColor(allLeds[b], this, "Выбор цвета");
    allLeds[b] = color;
    b->setStyleSheet("background-color: " + color.name() + ";");
    QCoreApplication::processEvents();
    customMode();
}

