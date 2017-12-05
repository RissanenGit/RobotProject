import threading
import time
import Queue

import RobotServer
import DatabaseConnection
import RobotMovement

import DataParser
import psutil

p=psutil.Process()
p.cpu_affinity([0])
print("mainThread running on core " + str(p.cpu_affinity()))

mainQueue = Queue.Queue()

serverThread = RobotServer.RobotServer(mainQueue)
movementThread = RobotMovement.RobotMovement(mainQueue)
dbThread = DatabaseConnection.DatabaseConnection(mainQueue)

serverThread.setDaemon(True)
serverThread.start()

movementThread.setDaemon(True)
movementThread.start()

dbThread.setDaemon(True)
dbThread.start()

def checkCommand(receivedData):
    print(receivedData)
    if(receivedData["Command"] == "Halt"):
        movementThread.insertMovementQueue(movementThread.setPanic)
    elif(receivedData["Command"] == "Release"):
        movementThread.insertMovementQueue(movementThread.removePanic)
    elif(receivedData["Command"] == "Return"):
        pass
        #movementThread.insertMovementQueue(movementThread.setReturn)
    elif(receivedData["Command"] == "SetSpeed"):
        movementThread.insertMovementQueue(movementThread.setMissionSpeed,int(receivedData["Value"]))
        #movementThread.insertMovementQueue(movementThread.setSpeed,int(receivedData["Value"]))
    elif(receivedData["Command"] == "RegisterRobot"):
        movementThread.insertMovementQueue(movementThread.robotAuth, receivedData["RobotId"],receivedData["RobotPassword"])
    elif(receivedData["Command"] == "GotoWaypoint"):
        movementThread.insertMovementQueue(movementThread.setWaypointTarget, str(receivedData["Value"]))

def checkQueue():
    try:
        data,source = mainQueue.get(timeout=0.05)
        if(type(source) == RobotServer.RobotServer or type(source) == DatabaseConnection.DatabaseConnection):
            checkCommand(DataParser.parseData(data))
        elif(type(source) == RobotMovement.RobotMovement):
            serverThread.insertServerQueue(serverThread.sendData,DataParser.createServerPacket(data))
            if(data["robotID"] != None):
                dbThread.insertDBQueue(dbThread.sendData,DataParser.createDatabasePacket(data))
    except Queue.Empty:
        return

while True:
    checkQueue()
    pass
