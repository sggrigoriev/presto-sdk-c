from sdk import Sdk
import http.client as http
import json, urllib

'''
Created on June 25, 2013
@author: Arun Varma
'''

class User(object):
    '''
    __init__
    defines a User object with a unique userId and list of locations where they have put devices
    @param userId: String
    @param locations: Location[]
    '''
    def __init__(self, username, password = None, appname = None, firstname = None, lastname = None):
        self.username = username
        self.password = password
        self.appname = appname
        self.firstname = firstname
        self.lastname = lastname

    '''
    getId
    returns the ID number of this user
    '''
    def getId(self):
        return self._userId

    '''
    getLocations
    returns this user's list of locations
    '''
    def getLocations(self):
        return self._locations

    '''
    addLocation
    adds a location to this User's list of locations
    @param loc: Location
    '''
    def addLocation(self, loc):
        self._locations.add(loc)

    '''
    createAccount
    creates an account for a new user
    @param: username: String
    @param password: String
    @param appName: String
    @param loc: Location
    @param firstName: String (optional)
    @param lastName: String (optional)
    '''
    def createAccount(self, username, password, appName, loc, firstName = None, lastName = None):
        conn = http.HTTPSConnection(Sdk.getRootSite())
        newUserExt = "/cloud/json/newUser"
        conn.connect()
        conn.request("POST", newUserExt, self.toJSON(), {
            "Content-Type: application/json"
        })
        result = json.loads(self._conn.getresponse().read())
        print(result)

    '''
    login
    returns the user's API Key
    @param username: String
    @param password: String
    '''
    def login(self, username, password):
        params = {}
        params["PASSWORD"] = password
        params["username"] = username
        urllib.request.urlopen(Sdk.getRootSite() + "/cloud/xml/login", urllib.parse.urlencode(params))

    '''
    toJSON
    returns the JSON representation of the object
    '''
    def toJSON(self):
        newDict = dict([(attr,self.__dict__[attr]) for attr in self.__dict__ if self.__dict__[attr]])
        return json.dumps(newDict, default=lambda obj: obj.__dict__, indent=2)