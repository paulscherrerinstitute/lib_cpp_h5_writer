from waitress import serve
import start_server
serve(start_server.app, host='0.0.0.0', port=9901)