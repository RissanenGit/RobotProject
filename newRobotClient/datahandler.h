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
    enum messageTypes{Halt,Return,Release,SetSpeed};
    enum eventType{Connected,Connecting,Disconnected,SentData,ReceivedData};
    enum robotEvents{Forward};

    int batteryLevel(){return _batteryLevel;}
    int speed(){return _speed;}
    QString action(){return _action;}

    void createMessage(messageTypes messageType, QString additionalData = NULL);
    void logEvent(eventType event, QList<QString> eventData = QList<QString>());
private:
    const int lowBatteryLevel = 30;

    int _batteryLevel = 0;
    int _speed = 0;
    QString _action = "";
    void checkBatteryLevel();
    QString getRobotEvent(int event);

    enum errorTypes{LowBattery};
    QMap<errorTypes,bool> errorList {{LowBattery,false}};

signals:
    void updateValues();
    void sendMessage(QByteArray data);
    void sendLogData(QString data);
    void lowBatteryWarning();
public slots:
    void parseData(QByteArray data);
};

#endif // DATAHANDLER_H
