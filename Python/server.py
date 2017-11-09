import socket
import dataparser

class Server:
    def __init__(self, address):
        self.connection = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.bufferSize = 1024
        self.address = address
        self.parser = dataparser.Dataparser()

    def listen(self):
        self.connection.bind(self.address)
        self.connection.listen(True)
        print("Started listening: ",self.connection.getsockname())

        while True:
            connection,address = self.connection.accept()
            print("Connection from: ", address)
            while True:
                data = connection.recv(self.bufferSize)
                if not data:
                    break
                else:
                    print("Received data: ",self.parser.parseData(data))
            print("Client disconnected: ", address)
            connection.close()

server = Server(('127.0.0.1',9999))
server.listen()
