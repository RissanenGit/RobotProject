#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeStatus(QString status)
{
    ui->statusLabel->setText(status);
}

void MainWindow::status(Connection::connectionStatus status)
{
    if(status == Connection::connectionStatus::Connected){
        connected = true;
        ui->connectButton->setEnabled(true);
        ui->connectButton->setText("Disconnect");
    }
    else if(status == Connection::connectionStatus::Disconnected){
        connected = false;
        ui->connectButton->setEnabled(true);
        ui->connectButton->setText("Connect");
    }
    else if(status == Connection::connectionStatus::Connecting){
        ui->connectButton->setEnabled(false);
        ui->connectButton->setText("Connecting");
    }
}
void MainWindow::on_connectButton_clicked()
{
    if(!connected){
        connection = new Connection(ui->addressField->text(),ui->portField->text().toInt());
        connect(connection,SIGNAL(statusChanged(QString)),this,SLOT(changeStatus(QString)));

        connect(connection,SIGNAL(connected(Connection::connectionStatus)),this,SLOT(status(Connection::connectionStatus)));
        qRegisterMetaType<Connection::connectionStatus>("Connection::connectionStatus"); /// ?????????+

        connection->start();
    }
    else{
        connection->closeConnection();
        delete connection;
        connection = nullptr;

    }


}
