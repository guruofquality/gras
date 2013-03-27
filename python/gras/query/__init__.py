import time
import BaseHTTPServer
import json
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
        args = server_registry[s.server]
        path = s.path
        if path.startswith('/'): path = path[1:]
        if not path: path = 'main.html'
        if path.endswith('.json'):
            s.send_response(200)
            s.send_header("Content-type", "application/json")
            s.end_headers()
            if path == 'args.json':
                arg_strs = dict((str(k), str(v)) for k, v in args.iteritems())
                s.wfile.write(json.dumps(arg_strs))
            elif path == 'stats.json':
                s.wfile.write(args['top_block'].query(s.path))
            else:
                s.wfile.write(json.dumps({}))
            return
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
    def __init__(self, args, **kwargs):
        server_class = BaseHTTPServer.HTTPServer
        self._httpd = server_class(args, MyHandler)
        server_registry[self._httpd] = kwargs

    def serve_forever(self):
        while True:
            try: self._httpd.serve_forever()
            except select.error: pass
