import socket
import threading
import time
import Queue

class RobotServer(threading.Thread):
    def __init__(self, mainQueue):
        super(RobotServer, self).__init__()

        self.serverSocket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.bufferSize = 1024
        self.queue = Queue.Queue() #Queue of this thread

        self.mainQueue = mainQueue #Queue of main thread

        self.dataToSend = None #Data to send

    def insertServerQueue(self, function, *args): #Function used to interact with thread queue
        self.queue.put((function, args))

    def checkQueue(self): #Used for checking the queue
        try:
            function, args= self.queue.get(timeout=0.5)
            function(*args)
        except Queue.Empty:
            return False

    def sendData(self,messageContents): #Called from main thread to send data
        self.dataToSend = messageContents

    def setData(self,data): #Sending data to main thread
        self.mainQueue.put(data)

    def run(self): #Main loop
        print("ServerThread started")
        self.serverSocket.bind(("127.0.0.1",9999))
        self.serverSocket.listen(True)
        while True: #Outer loop | Wait for client to connect
            connection,address = self.serverSocket.accept()
            connection.settimeout(1) #Set timeout for receiving data
            print("Connection from: ", address)

            while True: #Inner loop | Handle sending and receiving data to/from client
                self.checkQueue() #Check event queue
                try:
                    if self.dataToSend is not None: #If there is data to send
                        connection.send(self.dataToSend)
                        self.dataToSend = None #Set data to send to None

                    data = connection.recv(self.bufferSize)
                    if(len(data) > 0) : #If socket returns 0, client has disconnected
                        self.setData(data)
                    else: #Exit from inner while loop | wait for new client connection
                        connection.close()
                        break
                except socket.error as error: #Socket timeout in receiving data
                    pass
                time.sleep(1)

            print("Client disconnected: ", address) #Exited from inner loop, client disconnected
