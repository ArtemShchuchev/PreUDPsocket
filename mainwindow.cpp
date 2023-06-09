#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Настройка виджетов
    ui->pb_start->setDisabled(false);
    ui->pb_stop->setDisabled(true);
    ui->pb_sendData->setDisabled(true);
    ui->te_result->setReadOnly(true);


    udpWorker = new UDPworker(this);
    udpWorker->InitSocket();

    connect(udpWorker, &UDPworker::sig_sendTimeToGUI, this, &MainWindow::DisplayTime);
    connect(udpWorker, &UDPworker::sig_sendMesToGUI, this, &MainWindow::DisplayMes);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&]{

        QDateTime dateTime = QDateTime::currentDateTime();

        QByteArray dataToSend;
        QDataStream outStr(&dataToSend, QIODevice::WriteOnly);

        outStr << dateTime;

        udpWorker->SendDatagram(dataToSend);
        timer->start(TIMER_DELAY);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Кнопка: Начать передачу
void MainWindow::on_pb_start_clicked()
{
    timer->start(TIMER_DELAY);
    ui->pb_start->setDisabled(true);
    ui->pb_stop->setDisabled(false);
}


void MainWindow::DisplayTime(const QDateTime data)
{
    counter();
    ui->te_result->append("Текущее время: " + data.toString() + ". "
                          "Принято пакетов " + QString::number(counterPck));
}

void MainWindow::counter()
{
    counterPck++;
    if(counterPck % 20 == 0)
    {
        ui->te_result->clear();
    }
}

/*
    При нажатии на кнопку "Отправить датаграмму" приложение должно отправить
    введенный пользователем текст при помощи объекта класса QUdpSocket на адрес
    127.0.0.1 (localhost)

    Приложение должно принять эту датаграмму и отобразить в виджете "Принятые данные"
    сообщение вида: "Принято сообщение от адрес отправителя, размер сообщения(байт) Х
    Х - размер введенного в виджет текста
    Адрес отправителя - адрес с которого была осуществлена отправка датаграммы
*/
void MainWindow::DisplayMes(const MESSAGE message)
{
    counter();
    ui->te_result->append("Принято сообщение: " + message.mes +
                          ". От отправителя: " + message.adr +
                          ", размер сообщения " + message.size +
                          "(байт)");
}

// Кнопка: Остановить передачу
void MainWindow::on_pb_stop_clicked()
{
    timer->stop();
    ui->pb_start->setDisabled(false);
    ui->pb_stop->setDisabled(true);
}

// Слот запускается при изменении "le_textInp"
void MainWindow::on_le_textInp_textEdited(const QString &arg1)
{
    if (arg1 != "") ui->pb_sendData->setDisabled(false);
    else ui->pb_sendData->setDisabled(true);
}

// Слот запускается при вводе Return в поле "le_textInp"
void MainWindow::on_le_textInp_returnPressed()
{
    auto userTxt = ui->le_textInp->text();
    if (userTxt == "") return;

    ui->le_textInp->clear();
    ui->pb_sendData->setDisabled(true);

    QByteArray dataToSend;
    QDataStream outStr(&dataToSend, QIODevice::WriteOnly);

    outStr << userTxt;
    udpWorker->SendDatagram(dataToSend, MES_PORT);
}

// Кнопка: отправить датаграмму
void MainWindow::on_pb_sendData_clicked()
{
    on_le_textInp_returnPressed();
}

