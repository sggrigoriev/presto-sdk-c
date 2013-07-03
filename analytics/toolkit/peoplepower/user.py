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
    # specifies JSON as encoding language
    header = {"Content-Type" : "application/json"}
    # sends body and header to endpoint site as http "POST" command, receives response
    response = sdk.sendAndReceive("POST", endpoint, body, header)
    # verifies that Create Account was successful, reacts accordingly
    verifyResponse(response, "New user created in server with API Key: ")


'''
login
@return the User's API Key
@param username: String (email address)
@param password: String
'''
def login(username, password):
    endpoint = "/cloud/json/login"
    body = urllib.urlencode({"username" : username})
    header = urllib.urlencode({"PASSWORD" : password})
    # sends username and password to endpoint site as http "POST" command, receives response
    response = sdk.sendAndReceive("POST", endpoint, body, header)
    # verifies that Login was successful, reacts accordingly
    verifyResponse(response, "Logged in with the API Key: ")


'''
verifyResponse
verifies that action was successful, reacts accordingly
@param response: String
@param message: String
'''
def verifyResponse(response, message):
    # decode response from unicode to String, deserialize JSON code into a Python dictionary
    responseObj = json.loads(response.decode("utf-8"))
    # get result code from dictionary
    resultCode = responseObj["resultCode"]
    # if action was successful, get API Key
    if resultCode == sdk.SUCCESS:
        apiKey = responseObj["key"]
        print(message + apiKey)
    # else, throw an error corresponding to result code
    else:
        sdk.errorCode(resultCode)


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
        '''
        self.username = username
        self.password = password
        self.appname = appName
        self.location = loc
        self.firstname = firstName
        self.lastname = lastName
        '''


    '''
    getInfo
    returns a JSON object containing all known details about CachedUser and associated Locations
    '''
    def getInfo(self):
        endpoint = "/cloud/json/user"
        body = {}
        header = {"PRESENCE_API_KEY" : self.apiKey}
        # sends API Key to endpoint site as http "GET" command, receives response
        sdk.sendAndReceive("GET", endpoint, body, header)

    '''
    refreshFromServer
    refreshes CachedUser's information
    '''
    def refreshUser(self):
        endpoint = "/cloud/json/user"
        body = urllib.urlencode({"username" : self.username})
        header = {"PRESENCE_API_KEY" : self.apiKey}
        sdk.sendAndReceive("GET", endpoint, body, header)

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