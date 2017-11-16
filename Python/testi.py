import queue
import threading

class SomeClass(threading.Thread):
    def __init__(self,threadName, loop_time = 1.0/60):
        self.threadname = threadName
        self.q = queue.Queue()
        self.timeout = loop_time
        self.end = False
        self.loopcount = 0
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
            else:
                self.idle()

    def idle(self):
        self.loopcount = self.loopcount + 1
        pass

    def printData(self,data):
        print(data)
        pass

    def endThread(self):
        self.end = True
        pass
    def getLoopCount(self):
        return self.loopcount

import time

thread = SomeClass("Thread1")
thread.start()

toinenThread = SomeClass("Thread2")
toinenThread.start()

thread.printData("Data")
thread.endThread()

toinenThread.printData("Data2")
print(toinenThread.getLoopCount())
time.sleep(1)
print(toinenThread.getLoopCount())
toinenThread.endThread()
