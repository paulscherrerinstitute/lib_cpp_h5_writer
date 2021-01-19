# -*- coding: utf-8 -*-
#!/bin/env python
"""
Server that controls the PCO writer and communicates with the PCO client.
"""

# Notes
# =====
# 1.) This module uses numpy style docstrings, as they display nicely even
#       on the command line (for example, when using ipython's interactive
#       shell).
#
#   See:
#     http://sphinx-doc.org/latest/ext/napoleon.html
#     https://github.com/numpy/numpy/blob/master/doc/HOWTO_DOCUMENT.rst.txt
#
#   Use the napoleon Sphinx extension to render the docstrings correctly with
#   Sphinx: 'sphinx.ext.napoleon'
#

from flask import Flask, request, jsonify, session
from flask_session import Session
import requests
import copy
import json
import subprocess
import sys
import os

__author__ = 'Leonardo Hax Damiani'
__date_created__ = '2020-08-20'
__credits__ = 'Christian M. Schlepuetz'
__copyright__ = 'Copyright (c) 2020, Paul Scherrer Institut'
__docformat__ = 'restructuredtext en'

# path to writer's executable
tomcat_pco_writer = '/home/dbe/git/lib_cpp_h5_writer/tomcat/bin/tomcat_h5_writer'
# writer's rest api address:port

debug = False
# if not running on xbl-daq-32 -> DEBUG
# if os.uname()[1] != 'xbl-daq-34.psi.ch':
#     endpoint = 'http://localhost:9555'
#     debug = True


app = Flask(__name__)
# Configure session to use filesystem
app.config['SESSION_PERMANENT'] = True
app.config['SESSION_TYPE'] = 'filesystem'
app.config['previous_statistics'] = None
app.config['statistics'] = None
app.config['error'] = None
app.config['endpoint'] = 'http://xbl-daq-34:9555'
app.config['default_args'] = [
    'connection_address',
    'output_file',
    'n_frames',
    'user_id',
    'dataset_name',
    'max_frames_per_file',
    'writer_rest_port',
    'flask_api_address',
    ]

Session(app)



@app.route('/error', methods=['GET', 'POST'])
def error():
    """
    Method that saves/retrieves (GET/POST) writer's errors

    Returns
    -------
    response : dict
        An ack dict in case of POST and a dict with the error in case of GET.

    """
    if request.method == 'POST':
        app.config['error'] = request.json
        return {'success':True}
    if request.method == 'GET':
        try:
            error_content = app.config['error']
            if app.config['error'] is not None:
                get_error = copy.deepcopy(error_content)
                get_error['success'] =  True
                return get_error
        except Exception as e:
            return {'success':False, 'error':'unknown'}


@app.route('/finished', methods=['GET', 'POST'])
def finished():
    """
    Method that saves/retrieves (GET/POST) previous acquisitions
    statistics and status.

    Returns
    -------
    response : dict
        A dictionary that will have values depending on the method
        used (GET/POST) and if it was sucessful.

    """
    if request.method == 'POST':
        app.config['previous_statistics'] = request.json
        return {'success':True}
    if request.method == 'GET':
        try: 
            status_finished = app.config['previous_statistics']['status']
            previous_statistics = app.config['previous_statistics']
            if app.config['previous_statistics'] is not None:
                get_finish_statistics = copy.deepcopy(previous_statistics)
                get_finish_statistics['success'] =  True
                return get_finish_statistics
        except Exception as e:
            return {'success':False, 'status':'unknown'}

@app.route('/server_log', methods=['GET'])
def get_server_log():
    if request.method == 'GET':
        # Test with ssh service since pco_writer_1 is not running on debug machine
        if debug:
            outcome = '● pco_writer_1.service - pco_writer pco_writer_1\n   Loaded: loaded (/etc/systemd/system/pco_writer_1.service; static; vendor preset: disabled)\n   Active: active (running) since Fri 2020-09-25 16:04:06 CEST; 4 days ago\n Main PID: 33374 (bash)\n   CGroup: /system.slice/pco_writer_1.service\n           ├─33374 /bin/bash ./home/dbe/service_scripts/pco_writer_1_start.sh\n           └─33984 python /home/dbe/git/lib_cpp_h5_writer/tomcat/start_server.py\n\nSep 25 16:26:49 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:26:49] "GET /status HTTP/1.1" 200 -\nSep 25 16:26:49 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:26:49] "GET /status HTTP/1.1" 200 -\nSep 25 16:26:49 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:26:49] "GET /status HTTP/1.1" 200 -\nSep 25 16:26:49 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:26:49] "GET /status HTTP/1.1" 200 -\nSep 25 16:26:49 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:26:49] "GET /status HTTP/1.1" 200 -\nSep 25 16:27:18 xbl-daq-32.psi.ch bash[33374]: 129.129.95.54 - - [25/Sep/2020 16:27:18] "POST /finished HTTP/1.1" 200 -\nSep 25 16:27:31 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:27:31] "GET /status HTTP/1.1" 200 -\nSep 25 16:27:31 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:27:31] "GET /status HTTP/1.1" 200 -\nSep 25 16:27:35 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:27:35] "GET /status HTTP/1.1" 200 -\nSep 25 16:27:35 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:27:35] "GET /status HTTP/1.1" 200 -\n'
            log = outcome.split("\n\n")[1]
        else:
            log = subprocess.run(['systemctl', 'status', 'pco_writer_1'], stdout=subprocess.PIPE).stdout.decode('utf-8').split('\n\n')[1]
        return {'success':True, 'log': log}

@app.route('/server_uptime', methods=['GET'])
def get_server_uptime():
    if request.method == 'GET':
        # systemctl status pco_writer_1 | grep Active | awk '{ print $9 }'
        # Test with ssh service since pco_writer_1 is not running on debug machine
        if debug:
            outcome = '● pco_writer_1.service - pco_writer pco_writer_1\n   Loaded: loaded (/etc/systemd/system/pco_writer_1.service; static; vendor preset: disabled)\n   Active: active (running) since Fri 2020-09-25 16:04:06 CEST; 4 days ago\n Main PID: 33374 (bash)\n   CGroup: /system.slice/pco_writer_1.service\n           ├─33374 /bin/bash ./home/dbe/service_scripts/pco_writer_1_start.sh\n           └─33984 python /home/dbe/git/lib_cpp_h5_writer/tomcat/start_server.py\n\nSep 25 16:26:49 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:26:49] "GET /status HTTP/1.1" 200 -\nSep 25 16:26:49 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:26:49] "GET /status HTTP/1.1" 200 -\nSep 25 16:26:49 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:26:49] "GET /status HTTP/1.1" 200 -\nSep 25 16:26:49 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:26:49] "GET /status HTTP/1.1" 200 -\nSep 25 16:26:49 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:26:49] "GET /status HTTP/1.1" 200 -\nSep 25 16:27:18 xbl-daq-32.psi.ch bash[33374]: 129.129.95.54 - - [25/Sep/2020 16:27:18] "POST /finished HTTP/1.1" 200 -\nSep 25 16:27:31 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:27:31] "GET /status HTTP/1.1" 200 -\nSep 25 16:27:31 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:27:31] "GET /status HTTP/1.1" 200 -\nSep 25 16:27:35 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:27:35] "GET /status HTTP/1.1" 200 -\nSep 25 16:27:35 xbl-daq-32.psi.ch bash[33374]: 129.129.99.81 - - [25/Sep/2020 16:27:35] "GET /status HTTP/1.1" 200 -\n'
            uptime = outcome.split("Active: ")[1].split(";")[0]
        else:
            uptime = subprocess.run(['systemctl', 'status', 'pco_writer_1'], stdout=subprocess.PIPE).stdout.decode('utf-8').split("Active: ")[1].split(";")[0]
        return {'success':True, 'uptime': uptime}

@app.route('/status', methods=['GET'])
def get_status():
    """
    Gets the status from the PCO writer process or from a previous
    execution of it.

    The method will return previous run status and statistics or request
    a new status from the writer.

    Returns
    -------
    response : dict
        A dictionary with the following keys: success and value.

    """
    # verify if previous run is finished
    try:
        if app.config['previous_statistics']['status'] == 'finished' and app.config['previous_statistics'] is not None:
            get_finish_statistics = copy.deepcopy(app.config['previous_statistics'])
            get_finish_statistics['success'] =  True
            return get_finish_statistics
    except Exception as e:
        pass
    # gets new status from writer
    request_url = app.config['endpoint']+'/status'
    try:
        response = requests.get(request_url).json()
        if validate_response_from_writer(response):
            return validate_response_from_writer(response)
    except Exception as e:
        # if exception found -> check again for previous before returning unknown
        try:
            if app.config['previous_statistics']['status'] == 'finished' and app.config['previous_statistics'] is not None:
                get_finish_statistics = copy.deepcopy(app.config['previous_statistics'])
                get_finish_statistics['success'] =  True
                return get_finish_statistics
        except Exception as e:
            return {'success':True, 'status':'unknown'}
    return {'success':True, 'status':'unknown'}

@app.route('/ack', methods=['GET'])
def return_ack():
    """
    Method to simply check if the server is running.

    Returns
    -------
    response : dict
        A dictionary with the following keys: success.

    """
    if request.method == 'GET':
        return {'success':True}


@app.route('/start_pco_writer', methods=['POST'])
def start_pco_writer():
    """
    Starts the PCO writer process to start an acquisition.

    The method will verify if the provided arguments are
    matching the list of default arguments defined previously.
    The parameters that will be provided to the PCO writer process
    come via the POST request in the form of json.

    Returns
    -------
    response : dict
        A dictionary with the following keys: success and value.

    """
    if request.method == 'POST':
        request_json = request.data.decode()
        data = json.loads(request_json)
        is_ok = True
        for argument in app.config['default_args']:
            if argument not in data:
                value = ("Argument %s missing on the configuration file. "
                    "Please, check the configuration template file." % argument)
                is_ok = False
        if is_ok:
            tomcat_args = [tomcat_pco_writer]
            for key in app.config['default_args']:
                tomcat_args.append(data[key])
            app.config['endpoint'] = "http://xbl-daq-34:"+data['writer_rest_port']
            p = subprocess.Popen(
                tomcat_args,
                shell=False,
                stdin=None,
                stdout=None,
                stderr=None,
                close_fds=True,
                )
            # clear previous variables for the new execution
            app.config['previous_statistics'] = None
    return {'success':True}

def validate_response(server_response):
    """
    Method to validate the response from the server.

    Returns
    -------
    bool : bool
        True/False.
    """
    if not server_response['success']:
        return False
    print("\nPCO Writer trigger successfully submitted "
            "to the server. Retrieving writer's status...\n")
    return True

def validate_start_parameters(json_parameters):
    """
    Method to validate the start parameters from the client.

    Returns
    -------
    response : dict
        A dictionary with the following keys: success and value.
    """
    data = json.loads(json_parameters)
    for argument in app.config['default_args']:
        if argument not in data:
            value = ("Argument %s missing on the configuration file. "
                "Please, check the configuration template file." % argument)
            return {'success':False, 'status':value}
    return {'success':True, 'value':"OK"}

def validate_response_from_writer(writer_response):
    if 'status' in writer_response:
        return {'success':True, 
                'status':writer_response['status']}
    return {'success':False}

