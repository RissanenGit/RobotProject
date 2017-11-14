#include "connection.h"

Connection::Connection(QObject *parent, QString address, qint16 port) : QObject(parent){
    this->address = address;
    this->port = port;
}

Connection::~Connection(){ //Socket is deleted when connection is deleted in the main thread
        qDebug() << "Socket deleted";
        delete socket;
        socket = nullptr;
}


///Functions->
void Connection::connectSignals(){ //Handles connecting all the necessary signals
    connect(socket,SIGNAL(readyRead()),this,SLOT(readData()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnectedFromServer()));

    qDebug() << "Connected signals";
}

void Connection::emitSignal(connectionStatus status, QString statusText){ //Handles the updating of certain UI elements
    switch (status) {
    case connectionStatus::Connected:
        emit statusChanged(statusText);
        emit connectionStatusChanged(connectionStatus::Connected);
        break;
    case connectionStatus::Disconnected:
        emit statusChanged(statusText);
        emit connectionStatusChanged(connectionStatus::Disconnected);
        break;
    case connectionStatus::Connecting:
        emit statusChanged(statusText);
        emit connectionStatusChanged(connectionStatus::Connecting);
        break;
    default:
        break;
    }
}

bool Connection::retryConnection(){ //Handles reconnecting to the server
    for(int i = 0;i < retryCount; i++){ //Try (5) times to connect to the server

        qDebug() << "Retry " << i + 1;
        QThread::msleep(retryTimeout); //Sleep for 5 seconds, give time for the server to come up

        emitSignal(connectionStatus::Connecting,QStringLiteral("Connecting... (Retry: %1)").arg(i + 1));//Update UI on the reconnection status
        socket->connectToHost(address,port);
        if(socket->waitForConnected(5000)){
            qDebug() << "Connection re-established";
            emitSignal(connectionStatus::Connected,"Connected");
            connectSignals(); //Reconnect signals
            return true; //Connection re-established
        }
    }
    return false; //Can't re-establish connection
}

///Slots->
void Connection::createConnection(){ //Entrypoint
    qDebug() << "Creating connection";

    socket = new QTcpSocket();
    socket->connectToHost(address,port);

    emitSignal(connectionStatus::Connecting,"Connecting...");
    if(socket->waitForConnected(5000)){
        qDebug() << "Connected to server";
        emitSignal(connectionStatus::Connected,"Connected");
        connectSignals();//Connect signals
    }
    else{
        qDebug() << "Cant connect to server";
        if(!retryConnection()){ //Try to connect 5 times
            emitSignal(connectionStatus::Disconnected,"Cant connect to server");
            emit finished(); //Emitting finished, signal is allocated to threads quit slot, this will exit from the thread
        }
    }
}

void Connection::readData(){ //Handles the incoming data from server
    qDebug() << "Reading data from socket";
    emit dataReady(socket->readAll());
}

void Connection::sendData(QByteArray data){
    qDebug() << "Sending data to socket";
    socket->write(data);
}

void Connection::disconnectedFromServer(){ //Connection dropped
    qDebug() << "Disconnected from server, retry connection";
    if(!retryConnection()){//Try reconnecting
        qDebug("Cannot re-establish connection");
        emitSignal(connectionStatus::Disconnected,"Connection dropped");
        qDebug() << "Connection emitting finished";
        emit finished();//Exit from thread
    }

}

void Connection::closeConnection(){//User wants to disconnect through UI
    qDebug() << "Closing connection";

    disconnect(socket,SIGNAL(disconnected()),0,0); //Disconnecting, so socket wont emit disconnected()

    socket->close();

    qDebug() << "Connection emitting finished";
    emitSignal(connectionStatus::Disconnected,"Disconnected");
    emit finished();//Exit from thread
}
