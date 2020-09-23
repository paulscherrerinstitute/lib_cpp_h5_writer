import zmq
import datetime

def main():
    print("[", str(datetime.datetime.now()), "] ZMQ SUB script to consume zmq statistics packages generated by the Writer...")
    context = zmq.Context()
    # Socket
    socket = context.socket(zmq.SUB)
    socket.connect("tcp://localhost:8088")
    filter = b'statisticsWriter'
    print("[", str(datetime.datetime.now()), "] Filter: ", filter)
    socket.setsockopt(zmq.SUBSCRIBE, filter)

    while True:
        string = socket.recv()
        topic = string.split()
        print("[", str(datetime.datetime.now()), "] ", topic)




if __name__ == "__main__":
    main()