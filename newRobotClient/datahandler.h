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

private:
    int batteryLevel;
    QString task;
    QString action;
signals:

public slots:
    void parseData(QByteArray data);
};

#endif // DATAHANDLER_H
