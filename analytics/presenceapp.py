#!/usr/bin/python

'''
Created on August 2, 2013
@author: David Moss
'''
import argparse
import peoplepower.user as user
import peoplepower.device as device
from peoplepower.error import ApiError


'''
command line arguments
'''
parser = argparse.ArgumentParser()
parser.add_argument("appId", help="App ID")
parser.add_argument("--user", help="Presence Username / Email Address")
parser.add_argument("--password", help="Presence Password")
parser.add_argument("-v", "--verbose", help="Print more debug information to the console", action="store_true")
parser.add_argument("-s", "--server", help="developer or esp")
args = parser.parse_args()


'''
login
'''
try:
    us = user.login(args.user, args.password)
except ApiError as error:
    print(error.message)
    exit(1)


'''
print user's information if specified
'''
if(args.verbose):
    print(us)


'''
if app is registered to user, grab it
else, register the app in user's list of devices
'''
appId = int(args.appId)
appList = us.getDevicesByProductId(appId)
if not appList:
    deviceId = "17e64fb9-ceba-42d3-9ccc-567aee524aa1"#uuid.uuid4()
    app = device.register(us.getLocations().pop(), deviceId, appId)
    app.nickname("Morse Code")
else:
    app = appList.pop()


'''
import the given app and run it
'''
try:
    command_module = __import__("apps.%s.app" % args.appId, fromlist=["apps"])
except ImportError:
    print("Cannot find or run the given app")
command_module.run(us)