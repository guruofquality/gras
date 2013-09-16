import time
import BaseHTTPServer
import urlparse
import json
import os

__path__ = os.path.abspath(os.path.dirname(__file__))

server_registry = dict()

class MyHandler(BaseHTTPServer.BaseHTTPRequestHandler):

    #hide log messages to stdout by default
    def log_message(self, format, *args): pass

    def do_HEAD(s):
        s.send_response(200)
        s.send_header("Content-type", "text/html")
        s.end_headers()

    def do_GET(s):
        """Respond to a GET request."""

        #extract the path and set default
        o = urlparse.urlparse(s.path)
        args = server_registry[s.server]
        path = o.path

        query_args = dict([(k,v) for k,v in urlparse.parse_qs(o.query).iteritems()])

        #generate the topology png
        if path.endswith('dot.png'):
            s.send_response(200)
            s.send_header("Content-type", "image/png")
            s.end_headers()
            query_args['path'] = os.path.splitext(path)[0]
            json_args = json.dumps(query_args)
            dot_markup = args['top_block'].query(json_args)
            import subprocess
            dot_exe = os.environ.get("DOT_EXECUTABLE", "dot")
            p = subprocess.Popen(args=[dot_exe, "-T", "png"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            (stdout, stderr) = p.communicate(input=dot_markup)
            s.wfile.write(stdout)
            return

        #handle json requests
        if path.endswith('.json'):
            s.send_response(200)
            s.send_header("Content-type", "application/json")
            s.end_headers()
            if path == '/args.json':
                arg_strs = dict((str(k), str(v)) for k, v in args.iteritems())
                s.wfile.write(json.dumps(arg_strs))
            else:
                query_args['path'] = path
                json_args = json.dumps(query_args)
                s.wfile.write(args['top_block'].query(json_args))
            return

        #clean up path for filesystem
        if path.startswith('/'): path = path[1:]
        if not path: path = 'main.html'
        target = os.path.join(__path__, path)

        #get files from the local file system
        if os.path.exists(target):
            s.send_response(200)
            if target.endswith('.js'): s.send_header("Content-type", "text/javascript")
            elif target.endswith('.css'): s.send_header("Content-type", "text/css")
            else: s.send_header("Content-type", "text")
            s.end_headers()
            s.wfile.write(open(target).read())
        #otherwise not found do 404
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
