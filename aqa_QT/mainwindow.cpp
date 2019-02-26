#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    device.reset(new driver(ui->tvAllTimers, this));

    connect(device.data(), &driver::sendMessage, ui->statusBar, &QStatusBar::showMessage);
    connect(device.data(), &driver::sendEffectChanged, this, &MainWindow::refreshActions);

    connect(ui->pbExport,  &QPushButton::clicked, this, &MainWindow::exportTimers);
    connect(ui->pbImport,  &QPushButton::clicked, this, &MainWindow::importTimers);

    connect(ui->actIPPort, &QAction::triggered, this, &MainWindow::setIPPort);
    connect(ui->actConnect, &QAction::triggered, [this]{device->reconn();});


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
            if (device->effect() != 4) device->setEffect(4);
            device->setUserColor(0, color.toRgb().red(), color.toRgb().green(), color.toRgb().blue());
            ui->menu_7->setStyleSheet("QMenu::item { background-color: " + color.name() + "; }");
        });


    connect(ui->actCustomMode,  &QAction::triggered, [&]{device->setEffect(5);});

    connect(ui->pbUpload,  &QPushButton::clicked, this, &MainWindow::uploadToDevice);
    connect(ui->pbPickColor,  &QPushButton::clicked, this, &MainWindow::pickBrushColor);
    connect(ui->pbSave,  &QPushButton::clicked, this, &MainWindow::exportLedMap);
    connect(ui->pbLoad,  &QPushButton::clicked, this, &MainWindow::importLedMap);


    //timers
    connect(ui->pbAddTimer, &QPushButton::clicked, [&]{
        QScopedPointer<addTimerWin> win(new addTimerWin());
        connect(win.data(), &addTimerWin::sendStringTimer, device.data(), &driver::addTimer);
        win->exec();
        device->loadTimers(); //обновить таймеры
    });

    connect(ui->pbDelTimer, &QPushButton::clicked, [&]{
        if (ui->tvAllTimers->selectionModel() && ui->tvAllTimers->selectionModel()->hasSelection())
            device->removeTimer(ui->tvAllTimers->selectionModel()->selectedRows().at(0).data().toInt());
        device->loadTimers(); //обновить таймеры
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


    QTimer::singleShot(1000, this, &MainWindow::loadStatus); //load data
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


void MainWindow::uploadToDevice()
{
    for (int i = 0 ; i < allLeds.count() ; i++){
        if (device->effect() != 5) break; //выйти, если не тот эффект

        QColor c = allLeds.value(allLeds.keys().at(i));
        device->setUserColor(allLeds.keys().at(i)->text().toInt(), c.red(), c.green(),c.blue());
    }
}

void MainWindow::loadStatus()
{
    device->getStatus();
    QTimer::singleShot(50, this, &MainWindow::refreshActions);        // обновить элементы отображения
    QTimer::singleShot(50, [&]{device->loadTimers();});        // обновить список таймеров
}

void MainWindow::refreshActions()
{    ui->cbEnTimers->setChecked(device->isEnTimers());
     ui->actRelay0->setChecked(device->isRelay(0));
     ui->actRelay1->setChecked(device->isRelay(1));

     ui->actLedOff->setChecked(device->effect() == 0);
     ui->actSunrise->setChecked(device->effect() == 1);
     ui->actSunset->setChecked(device->effect() == 2);
     ui->actLedOn->setChecked(device->effect() == 3);
     ui->actSetUserColor->setChecked(device->effect() == 4);
     ui->actCustomMode->setChecked(device->effect() == 5);
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

            for (int i = 0; i < dataCount ; i++){
                QString loadedTimer;
                stream >> loadedTimer;
                device->addTimer(loadedTimer);
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

    const static QSize btnSize = QSize(22, 22);
    int row = 0;
    int column = 0;


    for (int i = ui->sbStart->value() ; i < ui->sbCount->value() ; i++){
        QPushButton* b = new QPushButton(QString::number(i), this);
        allLeds.insert(b, QColor(Qt::black));
        b->setFixedSize(btnSize);

        connect(b, &QPushButton::clicked, this, &MainWindow::setLedColor);

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


void MainWindow::setLedColor()
{
    QPushButton* b = qobject_cast<QPushButton*> (QObject::sender());

    allLeds[b] = brushColor;
    b->setStyleSheet("background-color: " + brushColor.name() + ";");

    device->setUserColor(b->text().toInt(), allLeds[b].red(), allLeds[b].green(), allLeds[b].blue());
}

void MainWindow::pickBrushColor()
{
    QPushButton* b = qobject_cast<QPushButton*> (QObject::sender());

    QColor returnColor = QColorDialog::getColor(brushColor, this, "Выбор цвета");
    if (returnColor.isValid()) brushColor = returnColor;
    b->setStyleSheet("background-color: " + brushColor.name() + ";");
}

void MainWindow::exportLedMap()
{
    if(allLeds.isEmpty()) return;

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Открыть файл с картой"),
                                                    QDir::homePath(),
                                                    tr("Файл карт (*.map);;Все файлы (*)"));

    if (!fileName.isEmpty()){
        QFile file(fileName);
        if(file.open(QIODevice::WriteOnly)){
            QDataStream stream(&file);
            stream.setVersion(QDataStream::Qt_4_2);

            stream << QString("Аквариумная карта");
            stream << allLeds.count(); //записать количество записей


            for (QPushButton* b : allLeds.keys()){
                stream << b->text() << allLeds[b];
            }

            if(stream.status() != QDataStream::Ok){
                QMessageBox::warning(nullptr, "Экспорт карты", "Ошибка экспорта");
            }else{
                QMessageBox::information(nullptr, "Экспорт карты", "Экспорт завершен успешно");
            }
        }
        file.close();
    }

}

void MainWindow::importLedMap()
{
    if(allLeds.isEmpty()) return;

    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Открыть файл с картой"),
                                QDir::homePath(),
                                tr("Файл карт (*.map);;Все файлы (*)"));
    if (!fileName.isEmpty()){
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly)){

            QDataStream stream(&file);
            stream.setVersion (QDataStream::Qt_4_2) ;

            QString check;
            stream >> check;
            if (check != "Аквариумная карта") {
                QMessageBox::warning(nullptr, "Импорт карты", "Это не файл карт");
                return;
            }

            int dataCount;
            stream >> dataCount;

            for (int i = 0; i < dataCount ; i++){
                QString ledName;
                stream >> ledName;
                for (QPushButton* b : allLeds.keys()){
                    if (b->text() == ledName){
                        stream >> allLeds[b];
                        b->setStyleSheet("background-color: " + allLeds[b].name() + ";");
                    }
                }
            }

            if(stream.status() != QDataStream::Ok){
                QMessageBox::warning(nullptr, "Импорт карты", "Ошибка импорта");
            }
            file.close();
        }
    }
}

