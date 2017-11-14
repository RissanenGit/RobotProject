#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QThread>

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr,QString address = "127.0.0.1", qint16 port = 9999);
    ~Connection();

    enum connectionStatus{Connected,Disconnected,Connecting};
private:
    QTcpSocket *socket;
    QString address;
    qint16 port;

    const int retryCount = 5;
    const int retryTimeout = 5000;

    void connectSignals();
    bool retryConnection();
signals:
    void connectionStatusChanged(Connection::connectionStatus status, QString statusText);
    void finished();
    void dataReady(QByteArray data);
public slots:
    void createConnection();
    void readData();
    void sendData(QByteArray data);
    void disconnectedFromServer();
    void closeConnection();

};

#endif // CONNECTION_H
