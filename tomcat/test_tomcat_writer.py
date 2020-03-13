import argparse

from mflow import mflow, BIND, PUSH
import numpy
import requests


def main():
    parser = argparse.ArgumentParser(description='Generate tomcat pco.edge test stream.')
    parser.add_argument("-o", '--stream_address', default="tcp://pc9808:9999", help="Address to bind the stream.")
    parser.add_argument("-i", '--n_images', default=100, help="Number of images to generate.")
    parser.add_argument('-m', '--n_modules', type=int, default=5, help="Number of modules to simulate.")

    arguments = parser.parse_args()

    stream_address = arguments.stream_address
    n_images = arguments.n_images
    shape = [n_modules * 512, 1024]
    dtype = "uint16"

    print("Use writer start command:")
    print("\t./tomcat_h5_writer [connection_address] [output_file] [n_frames] [rest_port] [user_id]")
    print("\t./tomcat_h5_writer tcp://pc9808:9999 test%d.h5 10 12000 -1")

    writer_parameters_url = "http://localhost:12000/parameters"
    writer_parameters = {
        "general/created": "right now",
        "general/user": "tester",
        "general/process": "test_writer.py",
        "general/instrument": "test script"
    }

    header = {
        "htype": 0,
        "tag": 0,
        "source": 1,
        "shape": shape,
        "frame": shape,
        "type": dtype,
    }

    image = numpy.random.rand(shape[0], shape[1]) * (2 ** 16)
    image = image.astype(dtype)

    stream = mflow.connect(address=stream_address, conn_type=BIND, mode=PUSH)

    for pulse_id in range(n_images):
        print("Sending pulse_id %d." % pulse_id)

        header["pulse_id"] = pulse_id
        header["frame"] = pulse_id

        stream.send(header, send_more=True, as_json=True)
        stream.send(image, send_more=False)


        # if pulse_id == 0:
        #     requests.post(url=writer_parameters_url, json=writer_parameters)


if __name__ == "__main__":
    main()