#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QDebug>
#include <QString>

class Connection : public QThread
{
public:
    Connection(QString address, qint16 port);
    ~Connection();

    bool createConnection();
    void closeConnection();
    void sendData(QByteArray data);
private:
    QTcpSocket *socket;
    QString address;
    qint16 port;

    // QThread interface
protected:
    virtual void run() override;
};

#endif // CONNECTION_H
