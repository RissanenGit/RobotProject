import threading
import RobotServer
import time
import Queue

mainQueue = Queue.Queue()

serverThread = RobotServer.RobotServer(mainQueue)
serverThread.setDaemon(True)
serverThread.start()

dataMuuttuja = ""

def checkQueue():
    try:
        print(mainQueue.get(timeout=0.5))
    except Exception:
        return

while True:
    checkQueue()
    serverThread.insertServerQueue(serverThread.setMessage,raw_input("Message: "))
    continue
