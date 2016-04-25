### run_camera.py ###
# - establishes connection to VCSBC360 camera on IP 192.168.0.65
# - uses telnet on port 2002 to start server program on camera and start video live mode
#   (see http://www.vision-components.com/en/ Getting Started document for telnet communication with VC cameras)
# This python script is meant for Odroid XU4, and should work for all VC cameras

import telnetlib
import os
import sys
import socket

# VCSBC360 default IP address and TCP/IP port number
HOST = "192.168.0.65"
SOCKET_PORT = 2002

telnet_timeout = 10 # seconds

### Ping IP address to initialize and obtain connection ###
### NOTE: Keep this section in here to ensure XU4 recognizes camera
ping_response = os.system("ping -c 3 " + HOST)

if ping_response == 0:
  print "\nVCSBC360 camera is connected\n"
else:
  print "\nERROR: VCSBC360 camera not connected. Check camera power ON, and ethernet cable\n"
  sys.exit()


### Setup camera using Telnet to run TCP server program ###
tn = telnetlib.Telnet(HOST,"23")

# write three carriage returns for login (+ margin)
tn.read_until("password ?")
tn.write("\r\n")
tn.read_until("\n")
tn.write("\r\n")
tn.read_until("\n")
tn.write("\r\n")

# read when shell is ready for user input
tn.read_until("$")

# start TCP server program img2b.c, and run in background
tn.write("img2b &\n")
print tn.read_until("port 2002", telnet_timeout)

# write another carriage return to go back to shell
tn.write("\r\n")
#print tn.read_until("\n")
#tn.write("\r\n")
#print tn.read_until("$")

# start live video mode
tn.write("vd -l\n")
print tn.read_until("live", telnet_timeout)
tn.write("\r\n")

print "Camera is running as server on port 2002, and is taking images\n"

# close Telnet connection
tn.close()
print "TCP Connection to VCSBC360 has been closed\n"
