import sys
import zmq
# Socket to talk to server
context = zmq.Context()
socket = context.socket(zmq.SUB)


#print "Collecting updates from weather server..."
socket.connect ("tcp://127.0.0.1:8088")
filter_msg = b'statisticsWriter'
socket.setsockopt(zmq.SUBSCRIBE, filter_msg)

# Process 5 updates
total_value = 0
while True:
    string = socket.recv()
    print(string)

# test