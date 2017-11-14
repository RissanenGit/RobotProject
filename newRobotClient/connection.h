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
    void emitSignal(connectionStatus status, QString statusText);
    bool retryConnection();
signals:
    void statusChanged(QString status);
    void connectionStatusChanged(Connection::connectionStatus status);
    void finished();
    void dataReady(QByteArray data);
public slots:
    void createConnection();
    void readData();
    void disconnectedFromServer();
    void closeConnection();
};

#endif // CONNECTION_H
