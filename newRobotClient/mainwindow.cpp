#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);

    //Connecting menu actions
    connect(ui->actionConnect,SIGNAL(triggered(bool)),this,SLOT(connectClicked())); //Connect button
    connect(ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(close())); //Exit button

    connect(ui->actionHalt,SIGNAL(triggered(bool)),this,SLOT(sendHalt())); //Halt Command Button
    connect(ui->actionReturn,SIGNAL(triggered(bool)),this,SLOT(sendReturn())); //Return Command Button
    connect(ui->actionRelease,SIGNAL(triggered(bool)),this,SLOT(sendRelease())); //Release Command Button
    connect(ui->actionSetSpeed,SIGNAL(triggered(bool)),this,SLOT(sendSpeed())); //SetSpeed Command Button
    connect(ui->actionRegisterRobot,SIGNAL(triggered(bool)),this,SLOT(sendRobotRegister())); //RegisterRobot Command Button

    connect(ui->actionAbout,SIGNAL(triggered(bool)),this,SLOT(showAbout())); //ShowAbout button
    connect(ui->actionUsage,SIGNAL(triggered(bool)),this,SLOT(showHelp())); //ShowHelp button

    connect(ui->actionSave_Log,SIGNAL(triggered(bool)),this,SLOT(saveLog())); //SaveLog button
    connect(ui->actionMovement_Controls,SIGNAL(triggered(bool)),this,SLOT(movementControl())); //MovementControlWindow button
    connect(ui->actionGoToWaypoint,SIGNAL(triggered(bool)),this,SLOT(sendGoToWaypoint()));
}

MainWindow::~MainWindow(){
    qDebug() << "Deleting MainWindow";
    delete ui;
    delete logDataToSave;
}

///Functions->
void MainWindow::connectSignals(){ //Connecting all the signals necessary for operation (thread,connection,handler)
    connect(thread,SIGNAL(started()),connection,SLOT(createConnection())); //thread->connection | When the thread is started, create the socket in connection

    connect(connection,SIGNAL(connectionStatusChanged(Connection::connectionStatus,QString)),this,SLOT(changeConnectionStatus(Connection::connectionStatus,QString))); //connection->this | For updating connection status in UI
    qRegisterMetaType<Connection::connectionStatus>("Connection::connectionStatus"); //Required for Connection::connectionStatus enum

    connect(this,SIGNAL(closeConnection()),connection,SLOT(closeConnection())); //this->connection | Handling closing of stuff inside connection

    connect(connection,SIGNAL(finished()),thread,SLOT(quit())); //connection->thread | Connection emits finished, thread quits
    connect(thread,SIGNAL(finished()),this,SLOT(threadFinished())); //thread->this | Thread emits finished, delete thread and connection

    connect(connection,SIGNAL(dataReady(QByteArray)),handler,SLOT(parseData(QByteArray))); //connection->handler | Data is ready at socket, send it to DataHandler

    connect(handler,SIGNAL(updateValues()),this,SLOT(updateUiValues())); //handler->this | Updating UI values
    connect(handler,SIGNAL(sendMessage(QByteArray)),connection,SLOT(sendData(QByteArray))); //handler->connection | For sending messages via socket

    connect(handler,SIGNAL(sendLogData(QString)),this,SLOT(updateLog(QString))); //handler->this | Upading the logview in UI
    connect(handler,SIGNAL(lowBatteryWarning()),this,SLOT(batteryLevelWarning())); //handler->this | If battery is below threshold, show a warning popup
}

void MainWindow::createConnection(){ //Creating objects and starting the thread
    thread = new QThread();
    connection = new Connection(nullptr,ipAddress,port);
    handler = new DataHandler();

    connection->moveToThread(thread);
    connectSignals();

    qDebug("Starting thread");
    thread->start();
}

///Slots->
void MainWindow::changeConnectionStatus(Connection::connectionStatus status,QString statusText){ //Called from connection, to update the connection status in the UI elements
    ui->statusLabel->setText(statusText);
    switch (status){ //Enable / Disable UI elements, based on connection status
    case Connection::Connecting:
        handler->logEvent(DataHandler::Connecting,QList<QString>{ipAddress + ":" + QString::number(port)});
        ui->actionConnect->setEnabled(false);
        ui->menuCommand->setEnabled(false);
        break;
    case Connection::Connected:
        handler->logEvent(DataHandler::Connected,QList<QString>{ipAddress + ":" + QString::number(port)});
        ui->actionConnect->setEnabled(true);
        ui->menuCommand->setEnabled(true);
        ui->actionConnect->setText("Disconnect");
        connected = true;
        break;
    case Connection::Disconnected:
        handler->logEvent(DataHandler::Disconnected,QList<QString>{ipAddress + ":" + QString::number(port)});
        ui->actionConnect->setEnabled(true);
        ui->menuCommand->setEnabled(false);
        ui->actionConnect->setText("Connect");
        connected = false;
        break;
    case Connection::ConnectionDropped:
        handler->logEvent(DataHandler::Disconnected,QList<QString>{ipAddress + ":" + QString::number(port),"Connection Lost"});
        ui->actionConnect->setEnabled(true);
        ui->menuCommand->setEnabled(false);
        ui->actionConnect->setText("Connect");
        connected = false;

        if(movementWindowOpen){ //If the connection drops, while the MovementControlWindow is open, close it
            movementControlWindow->close();
        }

        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, "Error", "Connection Lost.\nReconnect?",QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes){createConnection();} //Possibility to reconnect without re-entering IP and PORT
        break;
    default:
        break;
    }
}

void MainWindow::updateUiValues(){ //Updating values in the UI
    ui->batteryLabel->setText(QString::number((handler->batteryLevel())));
    ui->speedLabel->setText(QString::number(handler->speed()));
    ui->actionLabel->setText(handler->action());
    ui->robotIdLabel->setText(handler->robotId());
}
void MainWindow::updateLog(QString data){ //Updating the log in the UI
    ui->logView->append(data);
    logDataToSave->append(data + "\n");
}
void MainWindow::threadFinished(){ //Called from the connection, when we are exiting the thread
    while(!thread->isFinished()){} //Wait for thread to properely finish

    qDebug() << "Thread finished";

    delete thread;
    delete connection;
    delete handler;
}

void MainWindow::movementWindowAction(){ //Called when opening / closing the MovementControlWindow
    this->setEnabled(!this->isEnabled());
    movementWindowOpen = !movementWindowOpen;
}

void MainWindow::saveLog(){ //Called when pressing the "SaveLog" button. Handles reading the log file and appending new data into it
    if(logDataToSave->isEmpty()){ //No new data in the variable
        QMessageBox::warning(this, "Warning", "No new log data",QMessageBox::Yes);
        return;
    }
    QFile file("log.txt");
    bool hasDate = false;
    if(file.exists()){ //Used for reading the file for currentdate
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QMessageBox::warning(this, "Warning", file.errorString(),QMessageBox::Yes);
            return;
        }
        QTextStream in(&file);
        while(!in.atEnd()){
            if(in.readLine().contains(QDate::currentDate().toString("d.M.yyyy"))){
                hasDate = true;
                break;
            }
        }
        file.close();
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)){
        QMessageBox::warning(this, "Warning", file.errorString(),QMessageBox::Yes);
        return;
    }
    QTextStream out(&file);

    if(!hasDate){ //File has no date for currentDate, add it
        out << QDate::currentDate().toString("d.M.yyyy") << "\n----------------------------------------\n";
    }
    out << *logDataToSave << "\n"; //Write data
    file.close();
    *logDataToSave = ""; //Set data to save to none
    QMessageBox::information(this, "Success", "Log saved",QMessageBox::Yes);
}
void MainWindow::sendHalt(){handler->createMessage(DataHandler::Halt);} //Called when Halt Command Button is pressed
void MainWindow::sendReturn(){handler->createMessage(DataHandler::Return);}//Called when Return Command Button is pressed
void MainWindow::sendRelease(){handler->createMessage(DataHandler::Release);}//Called when Release Command Button is pressed
void MainWindow::sendSpeed(){
    bool ok;
    int speed = QInputDialog::getInt(this,"New speed:",tr("Speed:"), QLineEdit::Normal,0,handler->maxSpeed,1,&ok);
    if(!ok || !connected){return;}

    handler->createMessage(DataHandler::SetSpeed,QList<QString>{QString::number(speed)});
}

void MainWindow::sendRobotRegister(){//Called when RobotRegister Command Button is pressed
    bool ok;
    int robotID = QInputDialog::getInt(this,"Enter robot ID",tr("RobotID:"), QLineEdit::Normal,0,100,1,&ok);
    if(!ok || !connected){return;}
    QString robotPassword = QInputDialog::getText(this,"Enter robot password",tr("RobotPassword:"), QLineEdit::Password ,"",&ok);
    if(!ok || !connected){return;}

    handler->createMessage(DataHandler::RegisterRobot,QList<QString>{QString::number(robotID),robotPassword});
}

void MainWindow::sendGoToWaypoint()
{
    QInputDialog dialog;
    QStringList items(QList<QString>{"Etappi1","Etappi2","Etappi3"}); //Options in the dialog window

    dialog.setOptions(QInputDialog::UseListViewForComboBoxItems);
    dialog.setComboBoxItems(items);
    dialog.setWindowTitle("Choose waypoint");
    dialog.setFixedSize(100,100);

    if(dialog.exec() && connected){ //Exec returns 1 or true when OK is pressed, also check if connection is still active
        handler->createMessage(DataHandler::GoToWaypoint,QList<QString>{dialog.textValue()}); //Get selected value from dialog and send it
    }
}
void MainWindow::connectClicked(){//Called when Connect/Disconnect Button is pressed
    if(!connected){ //Disconnected, create new connection
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
    else{//Connected, disconnect
        emit closeConnection();
    }
}
void MainWindow::batteryLevelWarning(){QMessageBox::warning(this, "Warning", "Low battery on Robot",QMessageBox::Yes);} //Display low battery warning

void MainWindow::showHelp(){QMessageBox::information(this, "Help", "Connect to the Robot using the Connect button in the File menu.\n\nAfter connecting, send commands to the Robot using the commands found under the Commands menu.\n\nThe logs can be saved to a file using the Save Log button in the File menu.",QMessageBox::Yes);}
void MainWindow::showAbout(){ QMessageBox::information(this, "About", "This program is used to remotely control a Robot\n\nVersion 1.0",QMessageBox::Yes);}

void MainWindow::movementControl(){ //Open MovementControlWindow
    movementControlWindow = new ControlForm(nullptr,handler);
    movementControlWindow->setAttribute(Qt::WA_DeleteOnClose); //Delete window on closing
    connect(movementControlWindow,SIGNAL(destroyed(QObject*)),this,SLOT(movementWindowAction())); //After window is destroyed
    movementControlWindow->show();

    movementWindowAction();
}
