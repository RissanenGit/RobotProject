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
    case QRNodeFound:
        break;
    case PathBlocked:
        break;
    case SearchingWall:
        break;
    default:
        break;
    }
}

void DataHandler::createMessage(messageTypes messageType, QList<QString> additionalData)
{
    QByteArray message = "";
    switch (messageType) {
    case RegisterRobot:
        message = "Command:RegisterRobot,RobotId:";
        message += additionalData[0];
        message += ",RobotPassword:";
        message += additionalData[1];
        break;
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
        message += additionalData[0];
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
            QString contentValue = content[i].split(':')[1];
            if(contentData == "BatteryLevel"){
                _batteryLevel = contentValue.toFloat() / 1000;
            }
            else if(contentData == "EventType"){
                //_action = getRobotEvent(contentValue.toInt());
            }
            else if(contentData == "EventData"){
                _action = contentValue;
            }
            else if(contentData == "Speed"){
                _speed = contentValue.toInt();
            }
            else if(contentData == "robotID"){
                _robotId = contentValue;
            }
            else{
                qDebug() << "UnknownData" << contentData;
            }
            receivedData.append(contentData + ":" + content[i].split(':')[1]);
        }
    }
    logEvent(ReceivedData,receivedData);
    emit updateValues();

    checkBatteryLevel();

}
