import server
import _thread

def createServer(address,threadname):
    servu = server.Server(address,threadname)
    servu.listen()


_thread.start_new_thread(createServer,(('127.0.0.1',9999),"Thread1"))
_thread.start_new_thread(createServer,(('127.0.0.1',9998),"Thread2"))

while True:
    pass
