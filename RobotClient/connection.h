#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QDebug>
#include <QString>

class Connection : public QThread
{
    Q_OBJECT

public:
    Connection(QString address, qint16 port);
    ~Connection();

    bool createConnection();
    bool retryConnection();
    void closeConnection();
    void sendData(QByteArray data);

    enum connectionStatus{Connected,Disconnected,Connecting};

public slots:
    void serverDisconnected();
private:
    QTcpSocket *socket;
    QString address;
    qint16 port;
    const int retryCount = 5;
    const int retryTimeout = 5000;

signals:
    void statusChanged(QString status);
    void connected(Connection::connectionStatus status);
    // QThread interface
protected:
    virtual void run() override;
};

#endif // CONNECTION_H
