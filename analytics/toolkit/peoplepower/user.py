'''
user
Created on June 25, 2013
@author: Arun Varma
'''
import utilities, strings
import loc, device
import json


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
    endpoint = strings.USER
    # body is JSON representation of user, followed by JSON-representation of user's location
    body = '{"user": ' + utilities.toJson(userVitals) + ', "location": ' + utilities.toJson(loc) + '}'
    # specifies JSON as encoding language
    header = {strings.CONTENT_TYPE : strings.JSON_APPLICATION}
    # sends body and header to endpoint site as http "POST" command, receives response
    response = utilities.sendAndReceive(strings.POST, endpoint, body, header)
    responseObj = json.loads(response.decode(strings.DECODER))
    # verifies that Create Account was successful, reacts accordingly
    utilities.verifyResponse(responseObj)
    print('User "' + username + '" created')
    return User(responseObj["key"])


'''
login
@return the User's API Key
@param username: String
@param password: String
@param expiry: int (-1 if user does not wish for key to expire)
'''
def login(username, password, expiry = None):
    endpoint = strings.USER_LOGIN + username
    if expiry != None:
        endpoint += strings.EXPIRY + expiry
    header = {strings.PASSWORD : password}
    # sends username and password to endpoint site as http "POST" command, receives response
    response = utilities.sendAndReceive(strings.GET, endpoint, None, header)
    responseObj = json.loads(response.decode(strings.DECODER))
    # verifies that Login was successful, reacts accordingly
    utilities.verifyResponse(responseObj)
    print("Logged onto account " + username)
    return User(responseObj["key"])


'''
logout
@param user: User
logs user out of server by destroying API Key
'''
def logout(user):
    endpoint = strings.USER_LOGOUT
    header = {strings.API_KEY : user.getKey()}
    response = utilities.sendAndReceive(strings.GET, endpoint, None, header)
    # verifies that Login was successful, reacts accordingly
    utilities.verifyResponse(json.loads(response.decode(strings.DECODER)))
    print('User "' + user.getUsername() + '" logged out')


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
        self.refreshInfo()
        self.refreshDevices()

    '''
    refreshFromServer
    refreshes User's information from server
    '''
    def refreshInfo(self):
        info = self.getInfo()
        # extract information about user and cache it in the user object
        userInfo = info["user"]
        self.username = userInfo["userName"]
        # extract information about user's location and cache it in user object
        locDict = info["locations"].pop()
        self.loc = loc.toLoc(self, locDict)

    '''
    refreshDevices
    refreshes all of User's devices from server
    '''
    def refreshDevices(self):
        devInfo = self.getDeviceInfo()["devices"]
        # extract information about user's devices and cache it in user object
        self.devices = []
        while devInfo:
            curDev = device.toDevice(self, devInfo.pop())
            curDev.refreshInfo()
            self.devices.append(curDev)

    '''
    getInfo
    returns a JSON object from the server containing all known details about User and associated Locations
    '''
    def getInfo(self):
        endpoint = strings.USER
        body = None
        header = {strings.API_KEY : self.apiKey}
        # sends API Key to endpoint site as http "GET" command, receives response
        response = utilities.sendAndReceive(strings.GET, endpoint, body, header)
        responseObj = json.loads(response.decode(strings.DECODER))
        # verifies that Login was successful, reacts accordingly
        utilities.verifyResponse(responseObj)
        return responseObj

    '''
    getDeviceInfo
    returns a dictionary from the server containing all devices belonging to user
    '''
    def getDeviceInfo(self):
        endpoint = strings.DEVICES
        body = None
        header = {strings.API_KEY : self.apiKey}
        # sends API Key to endpoint site as http "GET" command, receives response
        response = utilities.sendAndReceive(strings.GET, endpoint, body, header)
        responseObj = json.loads(response.decode(strings.DECODER))
        # verifies that Login was successful, reacts accordingly
        utilities.verifyResponse(responseObj)
        return responseObj

    '''
    populateParams
    @return the most recent measurements of the user
    '''
    def populateParams(self):
        endpoint = strings.PARAMS
        body = None
        header = {strings.API_KEY : self.apiKey}
        # sends API Key to endpoint site as http "GET" command, receives response
        response = utilities.sendAndReceive(strings.GET, endpoint, body, header)
        responseObj = json.loads(response.decode(strings.DECODER))
        # verifies that Login was successful, reacts accordingly
        utilities.verifyResponse(responseObj)
        return responseObj

    '''
    getUsername
    @return this User's location
    '''
    def getUsername(self):
        return self.username

    '''
    getLoc
    @return this User's location
    '''
    def getLoc(self):
        return self.loc

    '''
    getKey
    @return the API Key of this User
    '''
    def getKey(self):
        return self.apiKey

    '''
    getDevices
    returns a list of devices belonging to the user
    '''
    def getDevices(self):
        return self.devices

    '''
    addDevice
    @param dev: Device
    adds a device object to user's list of devices
    '''
    def addDevice(self, dev):
        self.devices.append(dev)