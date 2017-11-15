#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);
}

MainWindow::~MainWindow(){
    delete ui;
}

///Functions->
void MainWindow::connectSignals(){
    connect(thread,SIGNAL(started()),connection,SLOT(createConnection()));

    connect(connection,SIGNAL(connectionStatusChanged(Connection::connectionStatus,QString)),this,SLOT(changeConnectionStatus(Connection::connectionStatus,QString))); //connection->this | For updating connection status in UI
    qRegisterMetaType<Connection::connectionStatus>("Connection::connectionStatus"); //Required for Connection::connectionStatus enum

    connect(this,SIGNAL(closeConnection()),connection,SLOT(closeConnection())); //this->connection | Handling closing of stuff inside connection

    connect(connection,SIGNAL(finished()),thread,SLOT(quit())); //connection->thread | Connection emits finished, thread quits
    connect(thread,SIGNAL(finished()),this,SLOT(threadFinished())); //thread->this | Thread emits finished, delete thread and connection

    connect(connection,SIGNAL(dataReady(QByteArray)),handler,SLOT(parseData(QByteArray))); //connection->handler | Data is ready at socket, send it to DataHandler

    connect(handler,SIGNAL(updateValues()),this,SLOT(updateUiValues())); //handler->this | Temporary for displaying values in UI
    connect(handler,SIGNAL(sendMessage(QByteArray)),connection,SLOT(sendData(QByteArray))); //handler->connection | Temporary for sending messages via socket

}


///Slots->
void MainWindow::changeConnectionStatus(Connection::connectionStatus status,QString statusText){
    ui->statusLabel->setText(statusText);
    switch (status){
    case Connection::connectionStatus::Connected:
        ui->connectButton->setEnabled(true);
        ui->connectButton->setText("Disconnect");
        connected = true;
        break;
    case Connection::connectionStatus::Disconnected:
        ui->connectButton->setEnabled(true);
        ui->connectButton->setText("Connect");
        connected = false;
        break;
    case Connection::connectionStatus::Connecting:
        ui->connectButton->setEnabled(false);
        ui->connectButton->setText("Connecting");
        break;
    default:
        break;
    }
}

void MainWindow::updateUiValues()
{
    //qDebug() << "Update ui";
    ui->batteryLabel->setText(QString::number((handler->batteryLevel())));
    ui->actionLabel->setText(handler->action());
    ui->taskLabel->setText(handler->task());
}
void MainWindow::threadFinished(){
    while(!thread->isFinished()){}

    qDebug() << "Thread finished";

    delete thread;
    delete connection;
    delete handler;
}


///Ui slots->
void MainWindow::on_connectButton_clicked(){
    if(!connected){
        thread = new QThread();
        connection = new Connection(nullptr,ui->connectionInfoLabel->text().split(":")[0],ui->connectionInfoLabel->text().split(":")[1].toInt());
        handler = new DataHandler();

        connection->moveToThread(thread);
        connectSignals();

        qDebug("Starting thread");
        thread->start();
    }
    else{
        emit closeConnection();
    }
}

void MainWindow::on_connectButton_2_clicked()
{
    //qDebug() << "TestiNappula pressed, sent message to server";

    handler->createMessage();
}
