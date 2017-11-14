#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);

    handler = new DataHandler();
}

MainWindow::~MainWindow(){
    delete ui;
}

///Functions->
void MainWindow::connectSignals(){
    connect(thread,SIGNAL(started()),connection,SLOT(createConnection()));

    connect(connection,SIGNAL(statusChanged(QString)),this,SLOT(changeStatusLabel(QString)));
    connect(connection,SIGNAL(connectionStatusChanged(Connection::connectionStatus)),this,SLOT(changeConnectionStatus(Connection::connectionStatus)));
    qRegisterMetaType<Connection::connectionStatus>("Connection::connectionStatus");

    //Finishing the thread
    connect(this,SIGNAL(closeConnection()),connection,SLOT(closeConnection()));

    connect(connection,SIGNAL(finished()),thread,SLOT(quit()));
    connect(thread,SIGNAL(finished()),this,SLOT(threadFinished()));

    connect(connection,SIGNAL(dataReady(QByteArray)),handler,SLOT(parseData(QByteArray)));

}


///Slots->
void MainWindow::changeStatusLabel(QString status){
    ui->statusLabel->setText(status);
}

void MainWindow::changeConnectionStatus(Connection::connectionStatus status){
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
void MainWindow::threadFinished(){
    while(!thread->isFinished()){}

    qDebug() << "Thread finished";

    delete thread;
    delete connection;
}


///Ui slots->
void MainWindow::on_connectButton_clicked(){
    if(!connected){
        thread = new QThread;
        connection = new Connection();

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
    qDebug() << "TestiNappula pressed";
}
