import socket, sys, random
from time import sleep
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    client.connect(("192.168.1.250", 80))
except socket.error:
    print("error connecting to ui")
    sys.exit(1)
client.send(("GET /raw HTTP1.1\n\n").encode())
try:
    txt = ""
    for i in range(0, 4):
        txt += "SETD^i."+str(i)+".mix^"+str(random.uniform(0, 1))+"\n"
    print(txt)
    sleep(1)
    client.send(txt.encode())
    client.close()
except KeyboardInterrupt:
    client.close()
