#include "datahandler.h"

DataHandler::DataHandler(QObject *parent) : QObject(parent){
}
DataHandler::~DataHandler(){}

void DataHandler::logEvent(eventType event, QList<QString> eventData)
{
    QString message = "";
    switch (event) {
    case Connected:
        message = "Connected to Server";
        break;
    case Connecting:
        message = "Connecting to Server";
        break;
    case Disconnected:
        message = "Disconnected from Server";
        break;
    case SentData:
        message = "Sent Data to Server";
        break;
    case ReceivedData:
        message = "Received Data From Server ";
        break;
    default:
        break;
    }
    for(int i = 0; i < eventData.length(); i++){
        message += " | " + eventData[i];
    }
    emit sendLogData(QStringLiteral("[%1] %2").arg(QDateTime::currentDateTime().toString("HH:mm:ss")).arg(message));
}

void DataHandler::checkBatteryLevel()
{
    if(_batteryLevel < lowBatteryLevel && errorList[LowBattery] == false){
        qDebug() << "LowBattery";
        errorList[LowBattery] = true;
        emit lowBatteryWarning();
    }
    else if(_batteryLevel > lowBatteryLevel + 20){
        errorList[LowBattery] = false;
    }
}

QString DataHandler::getRobotEvent(int event){
    switch (event) {
    case Forward:
        break;
    default:
        break;
    }
}

void DataHandler::createMessage(messageTypes messageType, QString additionalData)
{
    QByteArray message = "";
    switch (messageType) {
    case Halt:
        message = "Command:Halt";
        break;
    case Return:
        message = "Command:Return";
        break;
    case Release:
        message = "Command:Release";
        break;
    case SetSpeed:
        message = "Command:SetSpeed,Value:";
        message += additionalData;
    default:
        break;
    }
    logEvent(SentData,QList<QString>{message});
    emit sendMessage(message);
}

void DataHandler::parseData(QByteArray data){

    QList<QByteArray>content = data.split('\n');
    QList<QString>receivedData;
    for(int i = 0; i < content.length() - 1; i++){
        if(content[i].split(':').length() > 0){
            QString contentData = content[i].split(':')[0];
            if(contentData == "BatteryLevel"){
                _batteryLevel = content[i].split(':')[1].toInt();
            }
            else if(contentData == "EventType"){
                //_action = getRobotEvent(content[i].split(':')[1].toInt());
            }
            else if(contentData == "Speed"){
                _speed = content[i].split(':')[1].toInt();
            }
            else{
                qDebug() << "UnknownData" << content;
            }
            receivedData.append(contentData + content[i].split(':')[1]);
        }
    }
    logEvent(ReceivedData,receivedData);
    emit updateValues();

    checkBatteryLevel();

}
