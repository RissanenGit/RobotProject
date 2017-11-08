import socket

class Server:
    def __init__(self):
        self.connection = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.bufferSize = 1024
        self.address = '127.0.0.1',9999

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
                    break;
                else:
                    print("Received data: ", data)
            print("Client disconnected: ", address)
            connection.close()
