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


void MainWindow::on_pb_start_clicked()
{
    timer->start(TIMER_DELAY);
    ui->pb_start->setDisabled(true);
    ui->pb_stop->setDisabled(false);
}


void MainWindow::DisplayTime(QDateTime data)
{
    counterPck++;
    if(counterPck % 20 == 0)
    {
        ui->te_result->clear();
    }

    ui->te_result->append("Текущее время: " + data.toString() + ". "
                "Принято пакетов " + QString::number(counterPck));


}


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
    ui->le_textInp->clear();
    ui->pb_sendData->setDisabled(true);
}

// Кнопка: отправить датаграмму
void MainWindow::on_pb_sendData_clicked()
{
    on_le_textInp_returnPressed();
}

