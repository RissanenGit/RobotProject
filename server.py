import socket
import dataparser

class Server:
    def __init__(self, address,threadname):
        self.connection = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.bufferSize = 1024
        self.address = address
        self.parser = dataparser.Dataparser()
        self.threadname = threadname

    def listen(self):
        self.connection.bind(self.address)
        self.connection.listen(True)
        print(self.threadname,end=" : ")
        print("Started listening: ",self.connection.getsockname())

        while True:
            connection,address = self.connection.accept()
            print(self.threadname,end=" : ")
            print("Connection from: ", address)
            while True:
                data = connection.recv(self.bufferSize)
                if not data:
                    #Received timeout, close connection
                    break;
                else:
                    #Parse received data
                    print(self.threadname,end=" : ")
                    print("Received data: ",self.parser.parseData(data))

            #Client disconnected, close socket
            print(self.threadname,end=" : ")
            print("Client disconnected: ", address)
            connection.close()
