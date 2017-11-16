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

    connect(handler,SIGNAL(sendLogData(QString)),this,SLOT(updateLog(QString)));

}
///Slots->
void MainWindow::changeConnectionStatus(Connection::connectionStatus status,QString statusText){
    ui->statusLabel->setText(statusText);
    switch (status){
    case Connection::Connected:
        handler->logEvent(DataHandler::Connected,QList<QString>{ipAddress, "Testi"});
        ui->connectButton_2->setEnabled(true);
        ui->connectButton->setEnabled(true);
        ui->connectButton->setText("Disconnect");
        connected = true;
        break;
    case Connection::Disconnected:
        handler->logEvent(DataHandler::Disconnected,QList<QString>{ipAddress, "Testi", "Testi", "Testi"});
        ui->connectButton->setEnabled(true);
        ui->connectButton_2->setEnabled(false);
        ui->connectButton->setText("Connect");
        connected = false;
        break;
    case Connection::Connecting:
        handler->logEvent(DataHandler::Connecting,QList<QString>{ipAddress});
        ui->connectButton->setEnabled(false);
        ui->connectButton_2->setEnabled(false);
        ui->connectButton->setText("Connecting");
        break;
    default:
        break;
    }
}

void MainWindow::updateUiValues()
{
    ui->batteryLabel->setText(QString::number((handler->batteryLevel())));
    ui->actionLabel->setText(handler->action());
    ui->taskLabel->setText(handler->task());
}
void MainWindow::updateLog(QString data)
{
    ui->logView->append(data);
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

        ipAddress = ui->connectionInfoLabel->text().split(":")[0];
        port = ui->connectionInfoLabel->text().split(":")[1].toInt();

        thread = new QThread();
        connection = new Connection(nullptr,ipAddress,port);
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
    switch (ui->commandBox->currentIndex()) {
    case DataHandler::Halt:
        handler->createMessage(DataHandler::Halt);
        break;
    case DataHandler::Return:
        handler->createMessage(DataHandler::Return);
        break;
    default:
        break;
    }
}
