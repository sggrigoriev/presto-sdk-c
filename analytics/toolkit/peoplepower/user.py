'''
Created on June 25, 2013
@author: Arun Varma
'''
import sdk
import json
import urllib.parse as urllib


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
    body = '{"user": ' + sdk.toJson(userVitals) + ', "location":' + sdk.toJson(loc) + "}"
    # specify JSON as encoding language
    header = {"Content-Type" : "application/json"}
    # send body and header to endpoint site as http "POST" command, receives response
    response = sdk.sendAndReceive("POST", endpoint, body, header)
    # verify that Create Account was successful
    responseObj = json.loads(response.decode("utf-8"))
    sdk.verifyResponse(responseObj)
    print("New account was created as " + username)
    # extract key from response object and create new user object
    return CachedUser(responseObj["key"])


'''
login
@return the User's API Key
@param username: String (email address)
@param password: String
'''
def login(username, password):
    endpoint = "/cloud/json/login"
    body = urllib.urlencode({"username" : username})
    header = {"PASSWORD" : password}
    # send username and password to endpoint site as http "POST" command, receives response
    response = sdk.sendAndReceive("GET", endpoint, body, header)
    # verify that Login was successful
    responseObj = json.loads(response.decode("utf-8"))
    sdk.verifyResponse(responseObj)
    print("User logged in as " + username)
    # extract key from response object and create new user object
    return CachedUser(responseObj["key"])


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


class CachedUser(object):
    '''
    __init__
    defines a CachedUser object with a unique userId and list of locations where they have put devices
    @param apiKey: String
    '''
    def __init__(self, apiKey):
        self.apiKey = apiKey

        # extract username and id from server
        userInfo = self.getUserInfo()
        self.username = userInfo["userName"]
        self.id = userInfo["id"]

        # extract user's locations from server
        locationsInfo = self.getLocationsInfo()

    '''
    getAllInfo
    returns a JSON object containing all known details about CachedUser and associated Locations
    '''
    def getAllInfo(self):
        endpoint = "/cloud/json/user"
        body = {}
        header = {"PRESENCE_API_KEY" : self.apiKey}
        # sends API Key to endpoint site as http "GET" command, receives response
        response = sdk.sendAndReceive("GET", endpoint, body, header)
        responseObj = json.loads(response.decode("utf-8"))
        # verify that GET command was successful
        sdk.verifyResponse(responseObj)
        return responseObj

    '''
    refreshFromServer
    refreshes CachedUser's information
    '''
    def refreshUser(self):
        endpoint = "/cloud/json/user"
        body = {}
        header = {"PRESENCE_API_KEY" : self.apiKey}
        sdk.sendAndReceive("GET", endpoint, body, header)
        print(self.username + " refreshed")

    '''
    logout
    logs the user out by destroying their API Key
    '''
    def logout(self):
        endpoint = "/cloud/json/logout"
        body = urllib.urlencode({"username" : self.username})
        header = {"PRESENCE_API_KEY" : self.apiKey}
        response = sdk.sendAndReceive("GET", endpoint, body, header)
        responseObj = json.loads(response.decode("utf-8"))
        # verify that GET command was successful
        sdk.verifyResponse(responseObj)
        print(self.username + " logged out")

    '''
    getUserInfo
    returns a JSON object containing all known details about CachedUser
    '''
    def getUserInfo(self):
        # extract only user section of getAllInfo
        return self.getAllInfo()["user"]

    '''
    getLocationsInfo
    returns a JSON object containing all known details about associated locations
    '''
    def getLocationsInfo(self):
        # extract only locations section of getAllInfo
        return self.getAllInfo()["locations"]

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
    addLocation
    adds a location to this CachedUser's list of locations
    @param loc: Location
    '''
    def addLoc(self, loc):
        self.locations.add(loc)

    '''
    getLocations
    @return this CachedUser's list of locations
    '''
    def getLocs(self):
        return self.locations