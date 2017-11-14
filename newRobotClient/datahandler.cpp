#include "datahandler.h"

DataHandler::DataHandler(QObject *parent) : QObject(parent){
}
DataHandler::~DataHandler(){}

void DataHandler::parseData(QByteArray data){
    qDebug() << "Data from socket (DataHandler)";

    QList<QByteArray>content = data.split('\n');

    qDebug() << content;

}
