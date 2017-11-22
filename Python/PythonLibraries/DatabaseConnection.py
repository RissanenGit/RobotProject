import socket
import threading
import time
import Queue

class RobotServer(threading.Thread):
    def __init__(self, mainQueue):
        super(RobotServer, self).__init__()

        self.serverSocket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.serverSocket.setdefaulttimeout(2)

        self.dataConnection = (socket.AF_INET,socket.SOCK_STREAM)

        self.bufferSize = 1024
        self.queue = Queue.Queue() #Queue of this thread

        self.mainQueue = mainQueue #Queue of main thread

        self.dataToSend = None #Data to send

    def insertServerQueue(self, function, *args): #Function used to interact with server thread queue
        self.queue.put((function, args))

    def checkQueue(self): #Used for checking the queue
        try:
            function, args= self.queue.get(timeout=0.5)
            function(*args)
        except Queue.Empty:
            return False

    def sendData(self,data): #Called from main thread to send data
        self.dataToSend = data

    def setData(self,data): #Sending data to main thread
        self.mainQueue.put(data)

    def run(self): #Main loop
        print("ServerThread started")
        while True:
            self.checkQueue()
            self.serverSocket.bind(("0.0.0.0",9998))
            try:
                self.serverSocket.listen(True)
                connection,address = self.serverSocket.accept()
            except socket.timeout:
                if self.dataToSend != None:
                    self.dataConnection.connect("")
                    self.dataConnection.send(self.dataToSend)
                    self.dataConnection.close()
                continue
        while True: #Outer loop | Wait for client to connect
            print("Database connected from: ", address)
            connection.settimeout(0.5) #Set timeout for receiving data
            while True: #Inner loop | Handle sending and receiving data to/from client
                try:
                    data = connection.recv(self.bufferSize)
                    if(len(data) > 0) : #If socket returns 0, client has disconnected
                        self.setData(data)
                    else: #Exit from inner while loop | Wait for new client connection
                        connection.close()
                        break
                except socket.error: #Socket timeout in receiving data
                    pass
                time.sleep(1)
            print("Database disconnected: ", address) #Exited from inner loop, client disconnected
