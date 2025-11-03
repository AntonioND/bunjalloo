#!/usr/bin/python
"""
Basic HTTP server that will server files N times before exiting
"""
import BaseHTTPServer
from threading import Thread
import sys
import os

ext2type = {
        '.txt': 'text/plain',
        '.zip': 'application/zip',
        '.html': 'text/html',
}

class RequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    def do_GET(self):
        here = os.getcwd()
        p = here + self.path
        if os.path.exists(p):
            _, ext = os.path.splitext(p)
            self.send_response(200)
            self.send_header('Content-type', ext2type[ext])
            self.end_headers()
            data = open(p).read()
            self.wfile.write(data)
            self.wfile.flush()
        else:
            self.send_response(404)

    def log_message(self, format, *args):
        pass

class ServerThread(Thread):
    def __init__(self, times,
            server_class=BaseHTTPServer.HTTPServer,
            handler_class=RequestHandler):
        Thread.__init__(self)
        self.times = times
        self.server_class = server_class
        self.handler_class = handler_class

    def run(self):
        server_address = ('localhost', 8000)
        httpd = self.server_class(server_address, self.handler_class)
        times = self.times
        while times > 0:
            httpd.handle_request()
            times -= 1

if __name__ == '__main__':
    times = 1
    if len(sys.argv) > 1:
        times = int(sys.argv[1])
    thr = ServerThread(times)
    thr.start()
