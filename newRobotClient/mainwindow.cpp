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
    connect(ui->actionSave_Log,SIGNAL(triggered(bool)),this,SLOT(saveLog()));

}

MainWindow::~MainWindow(){
    delete ui;
    delete logDataToSave;
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
    connect(handler,SIGNAL(lowBatteryWarning()),this,SLOT(batteryLevelWarning()));

}

void MainWindow::createConnection()
{
    thread = new QThread();
    connection = new Connection(nullptr,ipAddress,port);
    handler = new DataHandler();

    connection->moveToThread(thread);
    connectSignals();

    qDebug("Starting thread");
    thread->start();
}

///Slots->
void MainWindow::changeConnectionStatus(Connection::connectionStatus status,QString statusText){
    ui->statusLabel->setText(statusText);
    switch (status){
    case Connection::Connecting:
        handler->logEvent(DataHandler::Connecting,QList<QString>{ipAddress + ":" + QString::number(port)});
        ui->menuConnect->setEnabled(false);
        ui->menuCommand->setEnabled(false);
        break;
    case Connection::Connected:
        handler->logEvent(DataHandler::Connected,QList<QString>{ipAddress + ":" + QString::number(port)});
        ui->menuConnect->setEnabled(true);
        ui->menuCommand->setEnabled(true);
        ui->actionConnect->setText("Disconnect");
        connected = true;
        break;
    case Connection::Disconnected:
        handler->logEvent(DataHandler::Disconnected,QList<QString>{ipAddress + ":" + QString::number(port)});
        ui->menuConnect->setEnabled(true);
        ui->menuCommand->setEnabled(false);
        ui->actionConnect->setText("Connect");
        connected = false;
        break;
    case Connection::ConnectionDropped:
        handler->logEvent(DataHandler::Disconnected,QList<QString>{ipAddress + ":" + QString::number(port),"Connection Lost"});
        ui->menuConnect->setEnabled(true);
        ui->menuCommand->setEnabled(false);
        ui->actionConnect->setText("Connect");
        connected = false;

        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, "Error", "Connection Lost.\nReconnect?",QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes){createConnection();}
        break;
    default:
        break;
    }
}

void MainWindow::updateUiValues(){
    ui->batteryLabel->setText(QString::number((handler->batteryLevel())));
    ui->speedLabel->setText(QString::number(handler->speed()));
    ui->actionLabel->setText(handler->action());
}
void MainWindow::updateLog(QString data)
{
    ui->logView->append(data);
    logDataToSave->append(data + "\n");
}
void MainWindow::threadFinished(){
    while(!thread->isFinished()){}

    qDebug() << "Thread finished";

    delete thread;
    delete connection;
    delete handler;
}
void MainWindow::saveLog(){
    if(logDataToSave->isEmpty()){
        QMessageBox::warning(this, "Warning", "No new log data",QMessageBox::Yes);
        return;
    }
    QFile file("log.txt");
    bool hasDate = false;
    //If file doesnt exist, create it
    if(file.exists()){
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QMessageBox::warning(this, "Warning", file.errorString(),QMessageBox::Yes);
            return;
        }
        QTextStream in(&file);
        while(!in.atEnd()){
            if(in.readLine().contains(QDate::currentDate().toString("d:M:yyyy"))){
                hasDate = true;
            }
        }
        file.close();
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)){
        QMessageBox::warning(this, "Warning", file.errorString(),QMessageBox::Yes);
        return;
    }
    QTextStream out(&file);

    if(!hasDate){
        out << QDate::currentDate().toString("d:M:yyyy") << "\n----------------------------------------\n";
    }
    out << *logDataToSave << "\n";
    file.close();
    *logDataToSave = "";
    QMessageBox::information(this, "Success", "Log saved",QMessageBox::Yes);
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
        bool ok;
        QString text = QInputDialog::getText(this, tr("Enter address"),tr("Address:"), QLineEdit::Normal,"192.168.1.63:9999",&ok);
        if(!ok){return;}

        QRegExp rx("[0-9]+(?:\.[0-9]+){3}:[0-9]+"); //Checking the IP and PORT
        QRegExpValidator validator (rx,0);
        int pos = 0;
        if(validator.validate(text,pos) != QValidator::Acceptable){
            QMessageBox::warning(this, "Error", "Invalid address entered",QMessageBox::Yes);
            return;
        }
        ipAddress = text.split(":")[0];
        port = text.split(":")[1].toInt();

        createConnection();
    }
    else{
        emit closeConnection();
    }
}
void MainWindow::batteryLevelWarning(){QMessageBox::warning(this, "Warning", "Low battery on Robot",QMessageBox::Yes);}

void MainWindow::showHelp(){QMessageBox::information(this, "Help", "Connect to the Robot using the Connect button in the File menu.\n\nAfter connecting, send commands to the Robot using the commands found under the Commands menu.",QMessageBox::Yes);}
void MainWindow::showAbout(){ QMessageBox::information(this, "About", "This program is used to remotely control a Robot\n\nVersion 1.0",QMessageBox::Yes);}
