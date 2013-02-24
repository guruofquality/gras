import time
import BaseHTTPServer

import os
__path__ = os.path.abspath(os.path.dirname(__file__))

get_stats_registry = [lambda: ""]

class MyHandler(BaseHTTPServer.BaseHTTPRequestHandler):

    def do_HEAD(s):
        s.send_response(200)
        s.send_header("Content-type", "text/html")
        s.end_headers()

    def do_GET(s):
        """Respond to a GET request."""
        if s.path.endswith('stats.xml'):
            s.send_response(200)
            s.send_header("Content-type", "text/xml")
            s.end_headers()
            s.wfile.write(get_stats_registry[0]())
            return
        s.send_response(200)
        s.send_header("Content-type", "text/html")
        s.end_headers()
        path = s.path
        if path.startswith('/'): path = path[1:]
        if not path: path = 'main.html'
        s.wfile.write(open(os.path.join(__path__, path)).read())

def http_server(args, get_stats_xml):
    get_stats_registry[0] = get_stats_xml
    server_class = BaseHTTPServer.HTTPServer
    httpd = server_class(args, MyHandler)
    return httpd
