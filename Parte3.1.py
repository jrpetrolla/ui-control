import socket, sys
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    client.connect(("192.168.1.250", 80))
except socket.error:
    print("error connecting to ui")
    sys.exit(1)
client.send(("GET /raw HTTP1.1\n\n").encode())
data = client.recv(1024)
print("Started")
fader = ""
idfd = 0
mute = -1
mute_old = -1
try:
    while True:
        data = client.recv(128)
        lines = data.decode().split('\n')
        for line in lines:
            if 'SETD^i.'+str(idfd)+'.mix' in line:
                if line != fader:
                    fader = line
                    number = fader.replace('SETD^i.'+str(idfd)+'.mix^',"")
                    try:
                        print(str(int(float(number)*100)) + '%')
                        #print(number)
                    except:
                        pass
            if 'SETD^i.'+str(idfd)+'.mute^' in line:
                mute = int(line.replace('SETD^i.'+str(idfd)+'.mute^',""))
                if mute_old != mute:
                    print("muted" if mute == 1 else "unmuted")
                    mute_old = mute
        client.send(("ALIVE\n").encode())
except KeyboardInterrupt:
    print("Finish")
    client.close()
    sys.exit(0)
