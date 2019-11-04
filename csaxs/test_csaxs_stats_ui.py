import zmq

print('receiving zmq stream...')
context = zmq.Context()
# Socket
socket = context.socket(zmq.SUB)
socket.connect("tcp://localhost:8088")
socket.setsockopt(zmq.SUBSCRIBE, b'statisticsWriter')

while True:
    string = socket.recv()
    topic = string.split()
    print(topic)
