import argparse

from mflow import mflow, BIND, PUSH
import numpy
import requests


def main():
    parser = argparse.ArgumentParser(description='Generate SF detector test stream.')
    parser.add_argument("-o", '--stream_address', default="tcp://127.0.0.1:8888", help="Address to bind the stream.")
    parser.add_argument("-i", '--n_images', default=100, help="Number of images to generate.")
    parser.add_argument('-m', '--n_modules', type=int, default=5, help="Number of modules to simulate.")

    arguments = parser.parse_args()

    stream_address = arguments.stream_address
    n_images = arguments.n_images
    n_modules = arguments.n_modules
    shape = [n_modules * 512, 1024]
    dtype = "uint16"

    print("Use writer start command:")
    print("\t./csaxs_h5_writer [connection_address] [output_file] [n_frames] [rest_port] [user_id] [n_modules] [statistic_monitor_address]")
    print("\t./csaxs_h5_writer tcp://127.0.0.1:8888 test%d.h5 10 12000 -1 1 tcp://127.0.0.1:8088")
    # print("\t./csaxs_h5_writer tcp://127.0.0.1:8888 test%d.h5 100 12000 -1 localhost 5 0 test_sf_writer 10")

    writer_parameters_url = "http://localhost:12000/parameters"
    writer_parameters = {
        "general/created": "right now",
        "general/user": "tester",
        "general/process": "test_writer.py",
        "general/instrument": "test script"
    }

    header = {
        "pulse_id": 0,
        "frame": 0,
        "is_good_frame": 1,
        "daq_rec": 1,
        "shape": shape,
        "type": dtype,

        "pulse_id_diff": [1] * n_modules,
        "framenum_diff": [1] * n_modules,
        "missing_packets_1": [1] * n_modules,
        "missing_packets_2": [1] * n_modules,
        "daq_recs": [1] * n_modules,
        "pulse_ids": [1] * n_modules,
        "framenums": [1] * n_modules,
        "module_number": [1] * n_modules,
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