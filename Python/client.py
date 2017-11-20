import socket

soketti = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
soketti.connect(('127.0.0.1',9999))

while True:
    try:
        print(soketti.recv(1024))
    except KeyboardInterrupt:
        soketti.close()
        print("Closing socket")
        exit()
    except socket.error:
        continue
