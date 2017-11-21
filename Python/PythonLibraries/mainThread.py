import threading
import RobotServer
import time
import Queue
import DataParser

mainQueue = Queue.Queue()

serverThread = RobotServer.RobotServer(mainQueue)
serverThread.setDaemon(True)
serverThread.start()

data = {"BatteryLevel" : 100, "Action" : "MovingLeft", "Task": "DeliveringItems"}

def checkQueue():
    try:
        print(mainQueue.get(timeout=0.5))
    except Queue.Empty:
        return

while True:
    checkQueue()
    serverThread.insertServerQueue(serverThread.sendData,DataParser.createDataPacket(data))
    data["BatteryLevel"] = data["BatteryLevel"] - 2
