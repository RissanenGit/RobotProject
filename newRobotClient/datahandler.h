#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>
#include <QDebug>
#include <QDateTime>

class DataHandler : public QObject
{
    Q_OBJECT
public:
    DataHandler(QObject *parent = nullptr);
    ~DataHandler();
    enum dataTypes{BatteryLevel,Action,Task};
    enum messageTypes{Halt,Return};
    enum eventType{Connected,Connecting,Disconnected,SentData,ReceivedData};

    int batteryLevel(){return _batteryLevel;}
    QString task() {return _task;}
    QString action(){return _action;}

    void createMessage(messageTypes messageType);
    void logEvent(eventType event, QList<QString> eventData = QList<QString>());
private:
    int _batteryLevel;
    QString _task;
    QString _action;
signals:
    void updateValues();
    void sendMessage(QByteArray data);
    void sendLogData(QString data);
public slots:
    void parseData(QByteArray data);
};

#endif // DATAHANDLER_H
