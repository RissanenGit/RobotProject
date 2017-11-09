import queue
import threading

class SomeClass(threading.Thread):
    def __init__(self,threadName, loop_time = 1.0/60):
        self.threadname = threadName
        self.q = queue.Queue()
        self.timeout = loop_time
        self.end = False
        super(SomeClass, self).__init__()

    def onThread(self, function, *args):
        newArgs = (self,)
        if len(args) > 0:
            newArgs = newArgs + args
        self.q.put((function, newArgs))

    def run(self):
        while True:
            if self.end:
                print(self.threadname + " : ENDING")
                break;
            try:
                function, args = self.q.get(timeout=self.timeout)
                function(*args)
            except queue.Empty:
                self.idle()

    def idle(self):
        pass

    def printData(self,data):
        print(data)
        pass

    def endThread(self):
        self.end = True
        pass


import time

thread = SomeClass("Thread1")
thread.start()

toinenThread = SomeClass("Thread2")
toinenThread.start()


thread.onThread(SomeClass.printData,"Datis")
toinenThread.onThread(SomeClass.printData,"Datis2")
thread.onThread(SomeClass.endThread)
toinenThread.onThread(SomeClass.endThread)