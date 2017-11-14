#include "connection.h"

Connection::Connection(QObject *parent, QString address, qint16 port) : QObject(parent){
    this->address = address;
    this->port = port;
}

Connection::~Connection(){
        delete socket;
        socket = nullptr;
}


///Functions->
void Connection::connectSignals()
{
    //connect(socket,SIGNAL(readyRead()),this,SLOT(readData()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnectedFromServer()));

    qDebug() << "Connected signals";
}

void Connection::emitSignal(Connection::connectionStatus status, QString statusText)
{
    switch (status) {
    case connectionStatus::Connected:
        emit statusChanged(statusText);
        emit connectionStatusChanged(Connection::connectionStatus::Connected);
        break;
    case connectionStatus::Disconnected:
        emit statusChanged(statusText);
        emit connectionStatusChanged(Connection::connectionStatus::Disconnected);
        break;
    case connectionStatus::Connecting:
        emit statusChanged(statusText);
        emit connectionStatusChanged(Connection::connectionStatus::Connecting);
        break;
    default:
        break;
    }
}

bool Connection::retryConnection()
{
    for(int i = 0;i < retryCount; i++){

        qDebug() << "Retry " << i + 1;
        QString teksti = QStringLiteral("Connecting... (Retry: %1)").arg(i + 1);
        emitSignal(connectionStatus::Connecting,teksti);

        QThread::msleep(retryTimeout);

        socket->connectToHost(address,port);
        if(socket->waitForConnected(5000)){
            qDebug() << "Connection re-established";
            emitSignal(connectionStatus::Connected,"Connected");
            return true;
        }
    }
    return false;
}

///Slots->
void Connection::createConnection()
{
    qDebug() << "Creating connection";

    socket = new QTcpSocket();
    socket->connectToHost(address,port);

    emitSignal(connectionStatus::Connecting,"Connecting...");
    if(socket->waitForConnected(5000)){
        qDebug() << "Connected to server";
        emitSignal(connectionStatus::Connected,"Connected");
        connectSignals();
    }
    else{
        qDebug() << "Cant connect to server";
        if(!retryConnection()){
            emitSignal(connectionStatus::Disconnected,"Disconnected");
            emit finished();
        }

    }
}

void Connection::readData()
{

}

void Connection::disconnectedFromServer()
{
    qDebug() << "Disconnected from server, retry connection";
    if(!retryConnection()){
        qDebug("Cannot re-establish connection");
        emitSignal(connectionStatus::Disconnected,"Disconnected");
        emit finished();
    }

}

void Connection::closeConnection()
{
    qDebug() << "Closing connection";

    disconnect(socket,SIGNAL(disconnected()),0,0); //Disconnecting, so socket wont emit disconnected()

    socket->close();

    qDebug() << "Connection emitting finished";
    emitSignal(connectionStatus::Disconnected,"Disconnected");
    emit finished();
}
