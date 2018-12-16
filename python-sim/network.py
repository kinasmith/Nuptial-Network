import random
import numpy
from scipy.spatial import distance
from threading import Timer
from time import sleep


network_size = 80
id_list = ['69', '34', 'b3', '72','19', '12', 'ad', '70', '3f', '0c', '09', 'bc','fc', '91', '20', '3a', '4e', '27', '01', '70','62', '1f', '10', '14', 'c7', 'aa', '9d', '95','08', 'b5', '4d', '80', 'c9', 'a2', '65', '53','4d', 'b1', '66', '8f', '2b', '48', '6f', 'f5','77', 'b7', 'de', '08', 'e5', '63', 'fd', 'f3','75', '65', '9e', 'ea', '71', '59', 'd5', '01','f2', '71', 'ba', 'dd', '0b', 'ed', 'e8', '46','bf', '7e', 'f3', 'db', '52', '43', '72', '09','56', 'a8', '6e', '62']
node = []
for i in range(network_size):
       node.append(0)

class RepeatedTimer(object):
    def __init__(self, interval, function, *args, **kwargs):
        self._timer     = None
        self.interval   = interval
        self.function   = function
        self.args       = args
        self.kwargs     = kwargs
        self.is_running = False
        self.start()

    def _run(self):
        self.is_running = False
        self.start()
        self.function(*self.args, **self.kwargs)

    def start(self):
        if not self.is_running:
            self._timer = Timer(self.interval, self._run)
            self._timer.start()
            self.is_running = True

    def stop(self):
        self._timer.cancel()
        self.is_running = False


class Node:
    'Common base class for all nodes'
    def __init__(self, uid, charge, location):
        self.uid = uid
        self.charge = charge
        self.location = location

    def displayNode(self):
        r = [self.uid, self.location, self.charge] 
        return r

    def getProximity(self):
        prox_nodes = []
        for i in range(network_size):
            if self.uid != node[i].uid:
                a = self.location
                b = node[i].location
                dist = distance.euclidean(a, b)
                if dist < 50:
                    prox_nodes.append(node[i].uid)
        return prox_nodes


for i in range(network_size):
    x = random.randint(0,500)
    y = random.randint(0,500)
    node[i] = Node(int(id_list[i], 16), random.randint(300,420)/10, [x,y])

# print("Total Node %d" % Node.nodeCount)
# for i in range(network_size):
    # print("ID:", node[i].displayNode()[0], "  \t ", node[i].displayNode()[1])

for i in range(5):
    print("Nodes near", node[i].uid, node[i].getProximity())



# def hello(name):
#     print("Hello %s!" % name)

# print("starting...")

rt = RepeatedTimer(1, node[i].getProximity) # it auto-starts, no need of rt.start()
try:
    sleep(5) # your long-running job goes here...
finally:
    rt.stop() # better in a try/finally block to make sure the program ends!

filename = id_list[0] + '.txt'
with open(filename, 'w') as file:
    file.write('whatever2')

# node Gateway:
    # 'Common base class for all Gateways'