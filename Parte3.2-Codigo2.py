from http.server import BaseHTTPRequestHandler, HTTPServer
import time, socket, sys

serverPort = 8742
uiserver = "192.168.1.250"
uiport = 80

muteMaster = "SETD^i.0.mute^{}\nSETD^i.1.mute^{}\nSETD^i.2.mute^{}\nSETD^i.3.mute^{}\nSETD^i.4.mute^{}\nSETD^i.5.mute^{}\n"
muteAux3 = "SETD^i.2.aux.2.mute^{}\nSETD^i.3.aux.2.mute^{}\nSETD^i.4.aux.2.mute^{}\nSETD^i.5.aux.2.mute^{}\nSETD^i.6.aux.2.mute^{}\nSETD^i.7.aux.2.mute^{}\nSETD^i.8.aux.2.mute^{}\nSETD^i.9.aux.2.mute^{}\n"

myCmds = {}
myCmds["1"] = muteMaster.replace("{}","0")
myCmds["2"] = muteMaster.replace("{}","1")
myCmds["3"] = muteAux3.replace("{}","0")
myCmds["4"] = muteAux3.replace("{}","1")
myCmds["altar"] = "SETD^i.0.mix^0.5635096566171613\nSETD^i.1.mix^0.37454833463775394\nSETD^i.2.mix^0.6040013684698913\n"

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
class MyServer(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path[1:] in myCmds:
            client.send(myCmds[self.path[1:]].encode())
            self.send_response(200)
            self.send_header("Content-type", "text/html")
            self.end_headers()
            self.wfile.write(bytes("","utf-8"))
        else:
            self.send_error(404)


if __name__ == "__main__":        
    try:
        client.connect((uiserver, uiport))
    except socket.error:
        print("error connecting to ui")
        sys.exit(1)
    client.send(("GET /raw HTTP1.1\n\n").encode())
    data = client.recv(1024)
    webServer = HTTPServer(('', serverPort), MyServer)
    print("Server started port: %s" % (serverPort))
    try:
        webServer.serve_forever()
    except KeyboardInterrupt:
        pass
    client.close()
    webServer.server_close()
    print("Server stopped.")
