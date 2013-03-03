import time
import BaseHTTPServer

import os
__path__ = os.path.abspath(os.path.dirname(__file__))

server_registry = dict()

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
            s.wfile.write(server_registry[s.server].get_stats(""))
            return
        path = s.path
        if path.startswith('/'): path = path[1:]
        if not path: path = 'main.html'
        target = os.path.join(__path__, path)
        if os.path.exists(target):
            s.send_response(200)
            if target.endswith('.js'): s.send_header("Content-type", "text/javascript")
            elif target.endswith('.css'): s.send_header("Content-type", "text/css")
            else: s.send_header("Content-type", "text")
            s.end_headers()
            s.wfile.write(open(target).read())
        else:
            s.send_response(404)
            s.send_header("Content-type", "text/html")
            s.end_headers()
            s.wfile.write("<p>not found</p>")

import select

class http_server(object):
    def __init__(self, args, top_block):
        server_class = BaseHTTPServer.HTTPServer
        self._httpd = server_class(args, MyHandler)
        server_registry[self._httpd] = top_block

    def serve_forever(self):
        while True:
            try: self._httpd.serve_forever()
            except select.error: pass
