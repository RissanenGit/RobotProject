#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>
#include <QDebug>

class DataHandler : public QObject
{
    Q_OBJECT
public:
    DataHandler(QObject *parent = nullptr);
    ~DataHandler();
    enum dataTypes{BatteryLevel,Action,Task};

    int batteryLevel(){return _batteryLevel;}
    QString task() {return _task;}
    QString action(){return _action;}

    void createMessage();

private:
    int _batteryLevel;
    QString _task;
    QString _action;
signals:
    void updateValues();
    void sendMessage(QByteArray data);
public slots:
    void parseData(QByteArray data);
};

#endif // DATAHANDLER_H
