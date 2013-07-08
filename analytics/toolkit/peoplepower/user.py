'''
Created on June 25, 2013
@author: Arun Varma
'''
import toolkit
import location
import json, urllib.parse as urllib


'''
createAccount
creates an account for a new User
@param: username: String (email address)
@param password: String
@param appName: String
@param email: String
@param loc: Location
@param firstName: String (optional)
@param lastName: String (optional)
'''
def createAccount(username, password, appName, email, loc, firstName = None, lastName = None):
    # put information into a JSON-convertible object
    userVitals = UserVitals(username, password, appName, email, firstName, lastName)
    endpoint = "/cloud/json/newUser"
    # body is JSON representation of user, followed by JSON-representation of user's location
    body = '{"user": ' + toolkit.toJson(userVitals) + ', "location":' + toolkit.toJson(loc) + "}"
    # specify JSON as encoding language
    header = {"Content-Type" : "application/json"}
    # send body and header to endpoint site as http "POST" command, receives response
    response = toolkit.sendAndReceive("POST", endpoint, body, header)
    # verify that Create Account was successful
    responseObj = json.loads(response.decode("utf-8"))
    toolkit.verifyResponse(responseObj)
    print("New account was created as " + username)
    # extract key from response object and create new user object
    return User(responseObj["key"])


'''
login
@return the User's API Key
@param username: String (email address)
@param password: String
@param expiry: int (optional)
'''
def login(username, password, expiry = None):
    endpoint = "/cloud/json/login"
    bodyDict = {"username" : username}
    if expiry != None:
        bodyDict["expiry"] = expiry
    body = urllib.urlencode(bodyDict)
    header = {"PASSWORD" : password}
    # send username and password to endpoint site as http "POST" command, receives response
    response = toolkit.sendAndReceive("GET", endpoint, body, header)
    # verify that Login was successful
    responseObj = json.loads(response.decode("utf-8"))
    toolkit.verifyResponse(responseObj)
    print("User logged in as " + username)
    # extract key from response object and create new user object
    return User(responseObj["key"])


class UserVitals(object):
    '''
    __init__
    defines a User object with only attributes necessary to be serialized as a JSON object
    @param username: String
    @param password: String
    @param appName: String
    @param email: String
    @param firstName: String (optional)
    @param lastName: String (optional)
    '''
    def __init__(self, username, password, appName, email, firstName = None, lastName = None):
        self.username = username
        self.password = password
        self.appName = appName
        self.email = email
        self.firstname = firstName
        self.lastname = lastName


class User(object):
    '''
    __init__
    defines a CachedUser object with a unique userId and list of locations where they have put devices
    @param apiKey: String
    '''
    def __init__(self, apiKey):
        self.apiKey = apiKey
        info = self.getInfo()

        # extract username and id from server
        userInfo = info["user"]
        self.username = userInfo["userName"]
        self.id = userInfo["id"]

        # extract user's locations from server
        locInfo = info["locations"]
        locsList = []
        # while list of location dictionaries contains elements
        while locInfo:
            # pop location dictionary from locations dictionary, convert to location object
            curLoc = location.toLoc(self, locInfo.pop())
            # add location object to user's list of locations
            locsList.append(curLoc)
        self.locs = locsList

    '''
    refreshFromServer
    refreshes CachedUser's information
    '''
    def refreshUser(self):
        endpoint = "/cloud/json/user"
        body = {}
        header = {"PRESENCE_API_KEY" : self.apiKey}
        toolkit.sendAndReceive("GET", endpoint, body, header)
        print(self.username + " refreshed")

    '''
    logout
    logs the user out by destroying their API Key
    '''
    def logout(self):
        endpoint = "/cloud/json/logout"
        body = urllib.urlencode({"username" : self.username})
        header = {"PRESENCE_API_KEY" : self.apiKey}
        response = toolkit.sendAndReceive("GET", endpoint, body, header)
        responseObj = json.loads(response.decode("utf-8"))
        # verify that GET command was successful
        toolkit.verifyResponse(responseObj)
        print(self.username + " logged out")
        del self

    '''
    getAllInfo
    returns a JSON object containing all known details about CachedUser and associated Locations
    '''
    def getInfo(self):
        endpoint = "/cloud/json/user"
        body = {}
        header = {"PRESENCE_API_KEY" : self.apiKey}
        # sends API Key to endpoint site as http "GET" command, receives response
        response = toolkit.sendAndReceive("GET", endpoint, body, header)
        responseObj = json.loads(response.decode("utf-8"))
        # verify that GET command was successful
        toolkit.verifyResponse(responseObj)
        return responseObj

    '''
    getId
    @return the ID number of this CachedUser
    '''
    def getId(self):
        return self.id

    '''
    getApiKey
    @return the API Key of this CachedUser
    '''
    def getApiKey(self):
        return self.apiKey

    '''
    getLocations
    @return this CachedUser's list of locations
    '''
    def getLocs(self):
        return self.locs

    '''
    addLocation
    adds a location to this CachedUser's list of locations
    @param loc: LocationVitals
    '''
    def addLoc(self, loc):
        endpoint = "/cloud/json/location"
        body = "{location:" + toolkit.toJson(loc) + "}"
        header = {"Content-Type" : "application/json", "PRESENCE_API_KEY" : self.apiKey}
        response = toolkit.sendAndReceive("POST", endpoint, body, header)
        # verify that GET command was successful
        toolkit.verifyResponse(json.loads(response.decode("utf-8")))