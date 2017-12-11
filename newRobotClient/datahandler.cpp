#include "datahandler.h"

DataHandler::DataHandler(QObject *parent) : QObject(parent){
}
DataHandler::~DataHandler(){}

void DataHandler::logEvent(eventType event, QList<QString> eventData)//Creating the string that gets shown in the UI log
{
    QString message = "";
    switch (event) {
    case Connected:
        message = "Connected to Robot";
        break;
    case Connecting:
        message = "Connecting to Robot";
        break;
    case Disconnected:
        message = "Disconnected from Robot";
        break;
    case SentData:
        message = "Sent Data to Robot";
        break;
    case ReceivedData:
        message = "Data from robot";
        break;
    default:
        break;
    }
    for(int i = 0; i < eventData.length(); i++){
        message += " | " + eventData[i];
    }
    emit sendLogData(QStringLiteral("[%1] %2").arg(QDateTime::currentDateTime().toString("HH:mm:ss")).arg(message));//Sending the data to the UI log slot
}

void DataHandler::checkBatteryLevel() //Checking if batterylevel is below a certain threshold, and the alarm has not yet been triggered
{
    if(_batteryLevel < lowBatteryLevel && errorList[LowBattery] == false){
        qDebug() << "LowBattery";
        errorList[LowBattery] = true; //Warning has been shown
        emit lowBatteryWarning(); //Showing low battery warning in the UI
    }
    else if(_batteryLevel > (lowBatteryLevel + 1)){ //If batterylevel is above a certain point, we can show the alarm again (Battery has been recharged)
        errorList[LowBattery] = false;
    }
}
void DataHandler::createMessage(messageTypes messageType, QList<QString> additionalData) //Creating the datastring sent to the robot via a socket
{
    QByteArray message = "";
    switch (messageType) { //Checking what type of command we are sending
    case RegisterRobot:
        message = "Command:RegisterRobot,RobotId:";
        message += additionalData[0]; //RobotId
        message += ",RobotPassword:";
        message += additionalData[1]; //RobotPW
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
        message += additionalData[0]; //Speed
        break;
    case GoToWaypoint:
        message = "Command:GotoWaypoint,Value:";
        message += additionalData[0]; //Waypoint
    default:
        break;
    }
    logEvent(SentData,QList<QString>{message}); //Logging the event
    emit sendMessage(message); //Sending data to the socket
}

void DataHandler::parseData(QByteArray data){ //Slot handles parsing the data received from the robot to the different variables

    QList<QByteArray>content = data.split('\n');
    if(content.length() == 0){return;}

    QList<QString>receivedData;
    for(int i = 0; i < content.length() - 1; i++){
        if(content[i].split(':').length() > 0){
            QString contentData = content[i].split(':')[0];
            QString contentValue = content[i].split(':')[1];
            if(contentData == "BatteryLevel"){
                _batteryLevel = (contentValue.toFloat() / 1000);
                receivedData.append(contentData + ":" + QString::number((_batteryLevel))); //Converting the voltage from mV to V
            }
            else if(contentData == "EventData"){
                _action = contentValue;
                receivedData.append(contentData + ":" + contentValue);
            }
            else if(contentData == "Speed"){
                _speed = contentValue.toInt();
                receivedData.append(contentData + ":" + contentValue);
            }
            else if(contentData == "robotID"){
                _robotId = contentValue;
                receivedData.append(contentData + ":" + contentValue);
            }
            else{ //Robot sent data that doesn't need to be shown in the UI
                qDebug() << "Data not wanted: " << contentData;
            }

        }
    }
    logEvent(ReceivedData,receivedData); //Logging the even
    emit updateValues(); //Updating values in the UI

    checkBatteryLevel(); //Checking the batterylevel

}
