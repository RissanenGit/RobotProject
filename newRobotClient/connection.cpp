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
    connect(socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(statusChanged(QAbstractSocket::SocketState)));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError(QAbstractSocket::SocketError)));

    qDebug() << "Connected signals";
}
void Connection::disconnectSignals(){
    disconnect(socket,SIGNAL(readyRead()),0,0);
    disconnect(socket,SIGNAL(disconnected()),0,0);
}

bool Connection::retryConnection(){ //Handles reconnecting to the server

    QThread::msleep(retryTimeout);
    for(int i = 0;i < retryCount; i++){ //Try (5) times to connect to the server
        qDebug() << "Retry " << i + 1;
        emit connectionStatusChanged(connectionStatus::Connecting,QStringLiteral("Connecting... (Retry: %1)").arg(i + 1));//Update UI on the reconnection status
        QThread::msleep(retryTimeout); //Sleep for 5 seconds, give time for the server to come up
        socket->connectToHost(address,port);
        if(socket->waitForConnected(5000)){
            qDebug() << "Connection re-established";
            emit connectionStatusChanged(connectionStatus::Connected,"Connected");
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

    emit connectionStatusChanged(connectionStatus::Connecting,"Connecting...");
    if(socket->waitForConnected(5000)){
        qDebug() << "Connected to server";
        emit connectionStatusChanged(connectionStatus::Connected,"Connected");
        connectSignals();//Connect signals
    }
    else{
        qDebug() << "Cant connect to server";
        if(!retryConnection()){ //Try to connect 5 times
            emit connectionStatusChanged(connectionStatus::Disconnected,"Cant connect to server");
            emit finished(); //Emitting finished, signal is allocated to threads quit slot, this will exit from the thread
        }
    }
}

void Connection::readData(){ //Handles the incoming data from server
    //qDebug() << "Reading data from socket";
    emit dataReady(socket->readAll());
}

void Connection::sendData(QByteArray data){
    qDebug() << "Sending data to socket: " << data;
    //qDebug() << "Socket: " << socket;
    //qDebug() << "Data address: " << &data;

    socket->write(data);
}

void Connection::disconnectedFromServer(){ //Connection dropped
    qDebug() << "Disconnected from server, retry connection";
    disconnectSignals();

    if(!retry){
        emit connectionStatusChanged(connectionStatus::ConnectionDropped,"Connection Lost");
        qDebug() << "Connection emitting finished";
        emit finished();//Exit from thread
    }
    if(retry && !retryConnection()){//Try reconnecting
        qDebug("Cannot re-establish connection");
        emit connectionStatusChanged(connectionStatus::ConnectionDropped,"Connection Lost");
        qDebug() << "Connection emitting finished";
        emit finished();//Exit from thread
    }

}

void Connection::closeConnection(){//User wants to disconnect through UI
    qDebug() << "Closing connection";

    disconnectSignals(); //Disconnecting, so socket wont emit disconnected()

    socket->close();

    qDebug() << "Connection emitting finished";
    emit connectionStatusChanged(connectionStatus::Disconnected,"Disconnected");
    emit finished();//Exit from thread
}

void Connection::statusChanged(QAbstractSocket::SocketState state)
{
    qDebug() << "Connection state: " << state;
}

void Connection::socketError(QAbstractSocket::SocketError error)
{
    qDebug() << "Socket error: " << error;
}

