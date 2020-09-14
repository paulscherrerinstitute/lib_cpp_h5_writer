#!/bin/env python

from flask import Flask, request, jsonify
import requests
import json
import subprocess
import sys
import os

# writer's executable
tomcat_pco_writer = '/home/dbe/git/lib_cpp_h5_writer/tomcat/bin/tomcat_h5_writer'
# writer's rest api address:port
# endpoint = 'http://xbl-daq-32:9555'
endpoint = 'http://localhost:9555'

default_args = ['connection_address', 'output_file', 'n_frames', 'user_id', 'dataset_name', 'max_frames_per_file']

app = Flask(__name__)

status_finished = 'unknown'
last_run_json = None

def validate_response(server_response):
    if not server_response['success']:
        print(server_response['value'])
        quit()
    print("\nPCO Writer trigger successfully submitted to the server. Retrieving writer's status...\n")
    return True

def validate_start_parameters(json_parameters):
    data = json.loads(json_parameters)
    for argument in default_args:
        if argument not in data:
            value = "Argument %s missing on the configuration file. Please, check the configuration template file." % argument
            return json.loads({'success':False, 'value':value})
    return {'success':True, 'value':"OK"}

def validate_response_from_writer(writer_response):
    val = {'success':True, 'value':writer_response['status']}
    return val

@app.route('/start_pco_writer', methods=['GET', 'POST'])
def start_pco_writer():
    if request.method == 'POST':
        request_json = request.data.decode()
        response = validate_start_parameters(request_json)
        if response["value"] == "OK":
            tomcat_args = [tomcat_pco_writer]
            args= json.loads(request_json)
            for key in default_args:
                tomcat_args.append(args[key])
            p = subprocess.Popen(tomcat_args,shell=False,stdin=None,stdout=None,stderr=None,close_fds=True)
        global status_finished
        status_finished = 'unknown'
        global last_run_json
        last_run_json = None
    return response

@app.route('/status', methods=['GET', 'POST'])
def get_status():
    global status_finished
    # verify if previous run is finished
    if status_finished == 'finished' and last_run_json is not None:
        return {'success':True, 'value':status_finished, 'written_frames': last_run_json['written_frames'], 'lost_frames':last_run_json['lost_frames'], 'end_time':last_run_json['end_time'], 'start_time':last_run_json['start_time'], 'duration':last_run_json['duration']}
    # gets new status from writer
    if request.method == 'GET':
        request_url = endpoint+'/status'
        try:
            response = requests.get(request_url).json()
            return validate_response_from_writer(response)
        except Exception as e:
            msg = 'initialized'
            return {'success':True, 'value':msg}

@app.route('/finished', methods=['GET', 'POST'])
def set_finished():
    global status_finished
    global last_run_json
    if request.method == 'POST':
        last_run_json = request.json
        status_finished = last_run_json['status']
        return {'success':True}
    else:
        if last_run_json is not None:
            return {'success':True, 'value':status_finished, 'written_frames': last_run_json['written_frames'], 'lost_frames':last_run_json['lost_frames'], 'end_time':last_run_json['end_time'], 'start_time':last_run_json['start_time'], 'duration':last_run_json['duration']}
        else:
            return {'success':True, 'value':status_finished}



if __name__ == '__main__':
    app.run(host='0.0.0.0', port=9901)
