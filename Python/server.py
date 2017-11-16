import socket
import dataparser
import time
import random

class Server:
    def __init__(self, address):
        self.connection = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.connection.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.bufferSize = 1024
        self.address = address
        self.parser = dataparser.Dataparser()

    def randomMessage(self):
        message = "0:" + str(random.randrange(1,100)) + "\n" + "1:"
        if(random.randrange(1,5) == 1):
            message = message + "MovingLeft\n"
        elif(random.randrange(1,5) == 2):
            message = message + "MovingRight\n"
        elif(random.randrange(1,5) == 3):
            message = message + "MovingForward\n"
        elif(random.randrange(1,5) == 4):
            message = message + "MovingBackward\n"
        else:
            message = message + "Stopped\n"

        message = message + "2:DeliverItems\n"

        return message

    def listen(self):
        self.connection.bind(self.address)
        self.connection.listen(True)
        print("Started listening: ",self.connection.getsockname())

        while True:
            connection,address = self.connection.accept()
            print("Connection from: ", address)
            while True:
                try:
                    while True:
                        connection.send(self.randomMessage())
                        try:
                            connection.settimeout(1)
                            connection.setblocking(0)
                            print(connection.recv(self.bufferSize))
                        except Exception:
                            time.sleep(1)
                            continue
                except KeyboardInterrupt:
                    connection.close()
                    self.connection.listen(False)
                    self.connection.close()
                    exit(0)
                    return
            print("Client disconnected: ", address)
            connection.close()

server = Server(('127.0.0.1',9999))
server.listen()
