import threading
import time
import Queue

import RobotServer
import DatabaseConnection
import RobotMovement

import DataParser

mainQueue = Queue.Queue()

serverThread = RobotServer.RobotServer(mainQueue)
movementThread = RobotMovement.RobotMovement(mainQueue)
dbThread = DatabaseConnection.DatabaseConnection(mainQueue)

serverThread.setDaemon(True)
serverThread.start()

movementThread.setDaemon(True)
movementThread.start()

dbThread.start()
dbThread.setDaemon(True)

#data = {"BatteryLevel" : 100, "Speed": 100, "Action" : "MovingLeft", "Task": "DeliveringItems"}
def checkCommand(receivedData):
    print(receivedData)
    if(receivedData["Command"] == "Halt"):
        movementThread.insertMovementQueue(movementThread.setPanic)
    elif(receivedData["Command"] == "Release"):
        pass
        #movementThread.insertMovementQueue(movementThread.removePanic)
    elif(receivedData["Command"] == "Return"):
        pass
        #movementThread.insertMovementQueue(movementThread.setReturn)
    elif(receivedData["Command"] == "SetSpeed"):
        pass
        #movementThread.insertMovementQueue(movementThread.setSpeed,int(receivedData["Value"]))

def checkQueue():
    try:
        data,source = mainQueue.get(timeout=0.1)
        if(type(source) == RobotServer.RobotServer or type(source) == DatabaseConnection.DatabaseConnection):
            checkCommand(DataParser.parseData(data))
        elif(type(source) == RobotMovement.RobotMovement):
            serverThread.insertServerQueue(serverThread.sendData,DataParser.createDataPacket(data))
            dbThread.insertDBQueue(dbThread.sendData,DataParser.createDatabasePacket(data))
    except Queue.Empty:
        return

while True:
    checkQueue()
    pass
