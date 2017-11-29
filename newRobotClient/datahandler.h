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
    enum messageTypes{Halt,Return,Release,SetSpeed,RegisterRobot};
    enum eventType{Connected,Connecting,Disconnected,SentData,ReceivedData};
    enum robotEvents{QRNodeFound,PathBlocked,SearchingWall};

    float batteryLevel(){return _batteryLevel;}
    int speed(){return _speed;}
    QString action(){return _action;}
    QString robotId(){return _robotId;}

    void createMessage(messageTypes messageType, QList<QString> additionalData = QList<QString>());
    void logEvent(eventType event, QList<QString> eventData = QList<QString>());
private:
    const float lowBatteryLevel = 10;

    float _batteryLevel = 0;
    int _speed = 0;
    QString _robotId = "None";
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
