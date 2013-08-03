#!/usr/bin/python

'''
Created on August 2, 2013
@author: David Moss
'''

import argparse
import peoplepower.user as user
import uuid

from peoplepower.error import ApiError

parser = argparse.ArgumentParser()
parser.add_argument("appId", help="App ID")
parser.add_argument("--user", help="Presence Username / Email Address")
parser.add_argument("--password", help="Presence Password")
parser.add_argument("-v", "--verbose", help="Print more debug information to the console", action="store_true")

args = parser.parse_args()

try:
    user = user.login(args.user, args.password)
except ApiError as error:
    print(error.message)
    exit(1)

if(args.verbose):
    print(user)

print(uuid.uuid4())
