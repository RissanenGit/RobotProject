#include "datahandler.h"

DataHandler::DataHandler(QObject *parent) : QObject(parent){
}
DataHandler::~DataHandler(){}

void DataHandler::createMessage()
{
    emit sendMessage("Command:HALT!");
}

void DataHandler::parseData(QByteArray data){
    //qDebug() << "Data from socket (DataHandler)";

    QList<QByteArray>content = data.split('\n');

    for(int i = 0; i < content.length() - 1; i++){
        if(content[i].split(':').length() > 0){
            switch (content[i].split(':')[0].toInt()) {
            case dataTypes::BatteryLevel:
                _batteryLevel = content[i].split(':')[1].toInt();
                break;
            case dataTypes::Action:
                _action = content[i].split(':')[1];
                break;
            case dataTypes::Task:
                _task = content[i].split(':')[1];
                break;
            default:
                break;
            }
        }
    }
    //qDebug() << "BatteryLevel:" << QString::number(_batteryLevel) << "Action:" << _action << "Task:" << _task;
    emit updateValues();

}
