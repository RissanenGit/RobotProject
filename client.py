import socket

soketti = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
soketti.connect(('127.0.0.1',int(input("Port: "))))

while True:
    try:
        soketti.send(input("Send: ").encode())
    except KeyboardInterrupt:
        soketti.close()
        exit()