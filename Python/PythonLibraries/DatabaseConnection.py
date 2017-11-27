import socket
import threading
import time
import Queue

DBAddress = "127.0.0.1"
DBPort = 9999

class DatabaseConnection(threading.Thread):
    def __init__(self, mainQueue):
        super(DatabaseConnection, self).__init__()

        self.serverSocket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.bufferSize = 1024

        self.queue = Queue.Queue() #Queue of this thread
        self.mainQueue = mainQueue #Queue of main thread

        self.dataToSend = None #Data to send

    def insertDBQueue(self, function, *args): #Function used to interact with server thread queue
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
        self.mainQueue.put((data,self))

    def run(self): #Main loop
        print("DBThread started")
        self.serverSocket.bind(("0.0.0.0",9998))
        self.serverSocket.settimeout(1)
        self.serverSocket.listen(True)
        while True:
            self.checkQueue()
            try:
                connection,address = self.serverSocket.accept()
            except socket.timeout: #No connection from DB
                if self.dataToSend != None: #If we want to send data to DB
                    dataConnection = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
                    dataConnection.connect((DBAddress,DBPort)) #IP of webserver
                    dataConnection.send(self.dataToSend)
                    dataConnection.close()
                    self.dataToSend = None
                continue

            print("DB connected from: ", address)
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
            print("DB disconnected: ", address) #Exited from inner loop, client disconnected
