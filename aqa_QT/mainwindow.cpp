#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    device.reset(new driver(this));
    device = new DeviceSM();
    tDevice = new QThread();
    device->moveToThread(tDevice);
    connect(tDevice, &QThread::started, device, &DeviceSM::start);


    ui->tvAllTimers->setModel(device->timersModel());
    ui->tvAllTimers->setColumnWidth(0,40);
    ui->tvAllTimers->setColumnWidth(1,50);
    ui->tvAllTimers->setColumnWidth(2,150);
    ui->tvAllTimers->setColumnWidth(3,250);
    ui->tvAllTimers->setColumnWidth(4,10);


    connect(device, &DeviceSM::sendStatus, ui->statusBar, &QStatusBar::showMessage);
    connect(device, &DeviceSM::ready, this, &MainWindow::refreshActions);

    connect(ui->pbExport,  &QPushButton::clicked, this, &MainWindow::exportTimers);
    connect(ui->pbImport,  &QPushButton::clicked, this, &MainWindow::importTimers);

    connect(ui->actIPPort, &QAction::triggered, this, &MainWindow::setIPPort);
    connect(ui->actConnect, &QAction::triggered, [&]{tDevice->start();});

    connect(ui->tbSyncTime, &QToolButton::clicked, [&]{device->setDateTime();});

    connect(ui->actRelay0, &QAction::triggered, [&](int ch){device->setRelay(0,ch);});
    connect(ui->actRelay1, &QAction::triggered, [&](int ch){device->setRelay(1,ch);});

    connect(ui->cbEnTimers, &QCheckBox::clicked, [&](int ch){device->setEnTimers(ch);});

    //effects
    for (QString name : device->effectsName()){
        QAction* act = new QAction(name, ui->menuEff);
        act->setCheckable(true);
        connect(act, &QAction::triggered, this, &MainWindow::setEffect);
        ui->menuEff->addAction(act);
    }

    connect(ui->actSetUserColor,&QAction::triggered, [&]{
            static QColor color = Qt::white;
            color = QColorDialog::getColor(color, this, "Выбор цвета");
            device->setOneColor(color.toRgb().red(), color.toRgb().green(), color.toRgb().blue());
            ui->menuColor->setStyleSheet("QMenu::item { background: " + color.name() + "; }");
        });




    //timers
    connect(ui->pbAddTimer, &QPushButton::clicked, [&]{
        QScopedPointer<addTimerWin> win(new addTimerWin(device));
        win->exec();
    });

    connect(ui->tvAllTimers, &QTableView::doubleClicked, [&](QModelIndex x){
        QScopedPointer<addTimerWin> win(new addTimerWin(device, x.siblingAtColumn(0).data().toInt()));
        win->exec();
    });

    connect(ui->pbDelTimer, &QPushButton::clicked, [&]{
        if (ui->tvAllTimers->selectionModel() && ui->tvAllTimers->selectionModel()->hasSelection())
            device->removeTimer(ui->tvAllTimers->selectionModel()->selectedRows().at(0).data().toInt());
    });


    //matrix
    connect(ui->pbMakeMatrix, &QPushButton::clicked, this, &MainWindow::makeMatrix);
    connect(ui->pbDelMatrix, &QPushButton::clicked, this, &MainWindow::delMatrix);
    connect(ui->pbUpload,  &QPushButton::clicked, this, &MainWindow::uploadToDevice);
    connect(ui->pbPickColor,  &QPushButton::clicked, this, &MainWindow::pickBrushColor);
    connect(ui->pbSave,  &QPushButton::clicked, this, &MainWindow::exportLedMap);
    connect(ui->pbLoad,  &QPushButton::clicked, this, &MainWindow::importLedMap);

    ui->sbCount->setValue(SETTING_INI->value("LEDS/count", 0).toInt());
    ui->sbStart->setValue(SETTING_INI->value("LEDS/start", 0).toInt());
    ui->sbElementsInRow->setValue(SETTING_INI->value("LEDS/columns", 0).toInt());

    //about
    connect(ui->actQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actAbout, &QAction::triggered, [&]{
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("О программе \"аквариум\"");
        msgBox.setTextFormat(Qt::RichText);   //this is what makes the links clickable
        msgBox.setText("Программа предназначена для управления подсветкой аквариума. \n"
                       "Исходный код программы можно скачать по ссылке <a href='https://github.com/Pichas/aqa_v2_wifi'>https://github.com/Pichas/aqa_v2_wifi</a> ");
        msgBox.exec();
    });

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
    if (allLeds.isEmpty()) return;

    for (int i = 0 ; i < allLeds.count() ; i++){
        QColor c = allLeds.value(allLeds.keys().at(i));
        device->setOneLedColor(allLeds.keys().at(i)->text().toInt(), c.red(), c.green(),c.blue());
    }
}



void MainWindow::refreshActions()
{
    ui->cbEnTimers->setChecked(device->isEnTimers());
    ui->actRelay0->setChecked(device->relay(0));
    ui->actRelay1->setChecked(device->relay(1));

    for (QAction* act : ui->menuEff->actions())
        act->setChecked(act->text() == device->curEffect());

    ui->lTime->setText(device->dateTime());
}

void MainWindow::exportTimers()
{

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Открыть файл с таймерами"),
                                                    QDir::homePath(),
                                                    tr("Файл таймеров (*.tim);;Все файлы (*)"));

    if(device->exportTimers(fileName)){
        QMessageBox::warning(nullptr, "Экспорт таймеров", "Ошибка экспорта");
    }else{
        QMessageBox::information(nullptr, "Экспорт таймеров", "Экспорт завершен успешно");
    }
}

void MainWindow::importTimers()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Открыть файл с таймерами"),
                                QDir::homePath(),
                                tr("Файл таймеров (*.tim);;Все файлы (*)"));

    if(device->importTimers(fileName)){
        QMessageBox::warning(nullptr, "Импорт таймеров", "Ошибка импорта");
    }else{
        QMessageBox::information(nullptr, "Импорт таймеров", "Импорт завершен успешно");
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

void MainWindow::setEffect()
{
    device->setEffect(qobject_cast<QAction*>(QObject::sender())->text());
}




void MainWindow::setLedColor()
{
    QPushButton* b = qobject_cast<QPushButton*> (QObject::sender());

    allLeds[b] = brushColor;
    b->setStyleSheet("background: " + brushColor.name() + ";");
    device->setOneLedColor(b->text().toInt(), allLeds[b].red(), allLeds[b].green(), allLeds[b].blue());
}

void MainWindow::pickBrushColor()
{
    QPushButton* b = qobject_cast<QPushButton*> (QObject::sender());

    QColor returnColor = QColorDialog::getColor(brushColor, this, "Выбор цвета");
    if (returnColor.isValid()) brushColor = returnColor;
    b->setStyleSheet("background: " + brushColor.name() + ";");
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
                        b->setStyleSheet("background: " + allLeds[b].name() + ";");
                    }
                }
                QCoreApplication::processEvents();
            }

            if(stream.status() != QDataStream::Ok){
                QMessageBox::warning(nullptr, "Импорт карты", "Ошибка импорта");
            }
            file.close();
        }
    }
}

