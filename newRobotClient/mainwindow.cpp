#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);

    //Connect menu actions
    connect(ui->actionConnect,SIGNAL(triggered(bool)),this,SLOT(connectClicked()));
    connect(ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(close()));

    connect(ui->actionHalt,SIGNAL(triggered(bool)),this,SLOT(sendHalt()));
    connect(ui->actionReturn,SIGNAL(triggered(bool)),this,SLOT(sendReturn()));
    connect(ui->actionRelease,SIGNAL(triggered(bool)),this,SLOT(sendRelease()));
    connect(ui->actionSetSpeed,SIGNAL(triggered(bool)),this,SLOT(sendSpeed()));

    connect(ui->actionAbout,SIGNAL(triggered(bool)),this,SLOT(showAbout()));
    connect(ui->actionUsage,SIGNAL(triggered(bool)),this,SLOT(showHelp()));

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

    connect(handler,SIGNAL(sendLogData(QString)),this,SLOT(updateLog(QString))); //For data logging

}
///Slots->
void MainWindow::changeConnectionStatus(Connection::connectionStatus status,QString statusText){
    ui->statusLabel->setText(statusText);
    switch (status){
    case Connection::Connected:
        handler->logEvent(DataHandler::Connected,QList<QString>{ipAddress, "Testi"});
        ui->menuConnect->setEnabled(true);
        ui->menuCommand->setEnabled(true);
        ui->actionConnect->setText("Disconnect");
        connected = true;
        break;
    case Connection::Disconnected:
        handler->logEvent(DataHandler::Disconnected,QList<QString>{ipAddress, "Testi", "Testi", "Testi"});
        ui->menuConnect->setEnabled(true);
        ui->menuCommand->setEnabled(false);
        ui->actionConnect->setText("Connect");
        connected = false;
        break;
    case Connection::Connecting:
        handler->logEvent(DataHandler::Connecting,QList<QString>{ipAddress});
        ui->menuConnect->setEnabled(false);
        ui->menuCommand->setEnabled(false);
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

void MainWindow::sendHalt(){handler->createMessage(DataHandler::Halt);}
void MainWindow::sendReturn(){handler->createMessage(DataHandler::Return);}
void MainWindow::sendRelease(){handler->createMessage(DataHandler::Release);}
void MainWindow::sendSpeed(){
    bool ok;
    QString message = QStringLiteral("New speed: (Current: %1 )").arg(handler->speed());
    int speed = QInputDialog::getInt(this,message,tr("Speed:"), QLineEdit::Normal,0,100,1,&ok);

    if(ok){
        handler->createMessage(DataHandler::SetSpeed,QString::number(speed));
    }

}


void MainWindow::connectClicked()
{
    if(!connected){
        QString text = QInputDialog::getText(this, tr("Enter IP:PORT"),tr("Address:"), QLineEdit::Normal);
        if(text.indexOf(':') == -1){
            QMessageBox msgBox;
            msgBox.setWindowTitle("Invalid address");
            msgBox.setText("Invalid address");
            msgBox.exec();
            return;
        }
        ipAddress = text.split(":")[0];
        port = text.split(":")[1].toInt();

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

void MainWindow::showHelp(){
    QMessageBox msgBox;
    msgBox.setText("How to use text here");
    msgBox.exec();
}

void MainWindow::showAbout(){
    QMessageBox msgBox;
    msgBox.setText("About text here");
    msgBox.exec();
}
