#!/bin/env python
from waitress import serve
import start_server
import sys
import os
from paste.translogger import TransLogger
from enum import Enum
import json
import argparse
from json.decoder import JSONDecodeError
import inspect
import jsonschema
from jsonschema import validate

__author__ = 'Leonardo Hax Damiani'
__date_created__ = '2021-01-14'
__credits__ = 'Christian M. Schlepuetz'
__copyright__ = 'Copyright (c) 2020, Paul Scherrer Institut'

# definition of the pco config json schema
pco_config_schema = {
    "description": "A representation of the pco camera configuration file.",
    "type": "object",
    "required": [ "cameras"],
    "properties": {
        "cameras":{
            "type": "array",
            "items": { "$ref": "#/definitions/cameras" }
        }
    },
    "definitions": {
        "cameras": {
            "type": "object",
            "required": [ "name", 
                        "connection_address", 
                        "flask_api_address", 
                        "writer_api_address" ],
            "properties": {
                    "name": {
                        "type": "string",
                        "description": "Name of the pco camera."
                    },
                    "connection_address": {
                        "type": "string",
                        "description": "Connection address of such camera."
                    },
                    "flask_api_address": {
                        "type": "string",
                        "description": "Flask api address that will serve such camera."
                    },
                    "writer_api_address": {
                        "type": "string",
                        "description": "Writer api address that "
                                "will be used for such camera."
                    },
            }
        }
    }
}


# validates pco cam json config file based on the schema
def validate_config(jsonData):
    try:
        validate(instance=jsonData, schema=pco_config_schema)
    except jsonschema.exceptions.ValidationError as err:
        return False
    return True

class NoTraceBackWithLineNumber(Exception):
    def __init__(self, msg):
        try:
            ln = sys.exc_info()[-1].tb_lineno
        except AttributeError:
            ln = inspect.currentframe().f_back.f_lineno
        self.args = "{0.__name__} (line {1}): {2}".format(type(self), ln, msg),
        sys.exit(self)

class NotAFileError(NoTraceBackWithLineNumber):
    pass

class NotAValidConfig(NoTraceBackWithLineNumber):
    pass

class CamNotFound(NoTraceBackWithLineNumber):
    pass

# check if config file exists
def check_file_path(string):
    if os.path.isfile(string):
        return string
    else:
        raise NotAFileError("Failed to load config file (%s). Please, provide a "
                    "valid configuration file." % string)

# parser
parser = argparse.ArgumentParser(
            description='Deployment server for the pco cameras operations at TOMCAT.',
            formatter_class=argparse.RawTextHelpFormatter)

parser.add_argument('-c', '--camera', metavar='N',  type=int, required=True,
        help='PCO camera id.')

parser.add_argument('-f', '--file', type=check_file_path,
        help='PCO configuration addresses.', default='./pco_config.json')


args = parser.parse_args()
json_cam_dict = None
with open(args.file) as f:
    json_cam_dict = json.load(f)
if not validate_config(json_cam_dict):
    raise NotAValidConfig("PCO configuration file not valid.")

# defines which camera will be used
cam_config = None
for camera in json_cam_dict['cameras']:
    if camera['name'] == "pco"+str(args.camera):
        cam_config = camera

if cam_config is None:
    raise CamNotFound("Configuration for camera %s could not be found on "
            "the config file (%s)." % (args.camera, args.file))

port = cam_config['flask_api_address'][-4:]
serve(TransLogger(start_server.app, setup_console_handler=False), port=port)
