#!/bin/env python
from waitress import serve
import start_server
import sys
from paste.translogger import TransLogger
from enum import Enum

class Cam(Enum):
    PCO3 = "PCO3", 9902
    PCO4 = "PCO4", 9901

args = [str(arg.upper()) for arg in sys.argv]
cam_arg = None

if len(args) != 2 and (Cam.PCO3.value[0] != args[0] and Cam.PCO4.value[0] != args[0]):
    print("\nPCO DAQ Server Error: PCO Camera not specified/recognized. Valid camera names: \"PCO3\" or \"PCO4\".\n")
    raise SystemExit("Usage: %s <PCO3/PCO4>" % sys.argv[0])
elif args[1] == Cam.PCO3.value[0]:
    cam_arg = Cam.PCO3
elif args[1] == Cam.PCO4.value[0]:
    cam_arg = Cam.PCO4
else:
    raise SystemExit("Usage: %s <PCO3/PCO4>" % sys.argv[0])

if cam_arg not None:
    serve(TransLogger(start_server.app, setup_console_handler=False), port=cam_arg.value[1])
