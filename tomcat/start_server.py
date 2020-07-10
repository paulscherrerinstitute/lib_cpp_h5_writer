#!/bin/env python

from flask import Flask, request, jsonify
import requests
import json
import subprocess
import sys

tomcat_pco_writer = '/home/dbe/git/lib_cpp_h5_writer/tomcat/bin/tomcat_h5_writer'
#if tomcat_pco_writer_path  not in sys.path:
#    sys.path.append(tomcat_pco_writer_path)

default_args = ['connection_address', 'output_file', 'n_frames', 'user_id', 'n_modules', 'rest_api_port', 'dataset_name', 'max_frames_per_file']

app = Flask(__name__)

def validate_start_parameters(json_parameters):
    data = json.loads(json_parameters)
    for argument in default_args:
        if argument not in data:
            value = "Argument %s missing on the configuration file. Please, check the configuration template file." % argument
            return json.loads({'success':False, 'value':value})
    return {'success':True, 'value':"OK"}


@app.route('/api/start_pco_writer', methods=['GET', 'POST'])
def start_pco_writer():
    if request.method == 'POST':
        request_json = request.data.decode()
        response = validate_start_parameters(request_json)
        if response["value"] == "OK":
            tomcat_args = [tomcat_pco_writer]
            args= json.loads(request_json)
            for key in default_args:
                tomcat_args.append(args[key])
            p = subprocess.run(tomcat_args)
            #p = multiprocessing.Process(target=tomcat_pco_writer, args=request.data.decode())
            #p.start()
        #p.join()
    elif request.method == 'GET':
        print("Verify if writer service is running... // todo ")

    return response

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=9900)
