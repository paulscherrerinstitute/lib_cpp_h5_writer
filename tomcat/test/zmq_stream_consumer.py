import sys
import zmq
import json
import base64
import os
import io
# from PIL import Image
from array import array


# Socket to talk to server
context = zmq.Context()
socket = context.socket(zmq.PULL)

print ("Collecting messages... ")
socket.connect ("tcp://pc9808:9999")

x=0
d = 0
while True:
    string = socket.recv()
    if x % 2 != 1:
        try:
            d = json.loads(string.decode())
            print(d)
        except UnicodeDecodeError as e:
            pass
    x+=1



