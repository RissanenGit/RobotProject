#include "connection.h"

Connection::Connection(QString address,qint16 port)
{
    this->address = address;
    this->port = port;
}

Connection::~Connection()
{
    if(socket != nullptr){
        delete socket;
        socket = nullptr;
    }
}

void Connection::run()
{
    qDebug() << "Thread started";
    if(createConnection()){
        //Connect signals and slots
        emit statusChanged("Connected");
        connect(socket,SIGNAL(disconnected()),this,SLOT(serverDisconnected()));
        this->exec();
    }
    else{
        emit statusChanged("Can't connect");
        closeConnection();
    }
}

bool Connection::createConnection(){

    socket = new QTcpSocket();
    socket->connectToHost(address,port);
    emit statusChanged("Connecting...");

    if(socket->waitForConnected(5000)){
        qDebug() << "Connected to robot";
        emit connected(connectionStatus::Connected);
        return true;
    }
    else{
        qDebug() << "Cant connect to robot";

        delete socket;
        socket = nullptr;

        return false;
    }
}

bool Connection::retryConnection()
{
    emit connected(connectionStatus::Connecting);
    for(int i = 0;i < retryCount;i++){
        qDebug() << "Retry: " << i;
        if(createConnection()){
            return true;
        }
        QString retryText = QStringLiteral("Connecting... (Retry: %1)").arg(i);
        emit statusChanged(retryText);
        msleep(retryTimeout);
    }
    return false;
}

void Connection::closeConnection()
{
    if(socket != nullptr){
        socket->close();

        delete socket;
        socket = nullptr;
    }

    if(!retryConnection()){

        emit statusChanged("Disconnected");
        emit connected(connectionStatus::Disconnected);
        exit(0);
    }

}

void Connection::sendData(QByteArray data)
{
    socket->write(data);
}


//SLOTS
void Connection::serverDisconnected()
{
    qDebug() << "Server disconnected";
    closeConnection();
}
