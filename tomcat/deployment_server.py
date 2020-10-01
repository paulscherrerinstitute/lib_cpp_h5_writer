#!/bin/env python
from waitress import serve
import start_server
from paste.translogger import TransLogger
serve(TransLogger(start_server.app, setup_console_handler=False), port=9901)
