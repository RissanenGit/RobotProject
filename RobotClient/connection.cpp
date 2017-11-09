#include "connection.h"

Connection::Connection(QString address,qint16 port)
{
    this->address = address;
    this->port = port;
}

Connection::~Connection()
{
    qDebug() << "Destroying connection";
    delete socket;
    socket = nullptr;
}

void Connection::run()
{
    qDebug() << "Thread started";
    if(createConnection()){
        //Connect signals and slots
        this->exec();
    }
    else{
        closeConnection();
    }
}

bool Connection::createConnection(){

    socket = new QTcpSocket();
    socket->connectToHost(address,port);
    if(socket->waitForConnected(5000)){
        qDebug() << "Connected to robot";
        return true;
    }
    else{
        qDebug() << "Cant connect to robot";
        return false;
    }
}

void Connection::closeConnection()
{
    if(socket->isOpen()){
        socket->close();
    }
    exit(0);

}

void Connection::sendData(QByteArray data)
{
    socket->write(data);
}
