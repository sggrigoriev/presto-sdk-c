'''
user
Created on June 25, 2013
@author: Arun Varma
'''
import peoplepower.utilities as utilities
import peoplepower.strings as strings
import peoplepower.loc as loc
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
    response = utilities.sendAndReceive(strings.HTTP_POST, endpoint, body, header)
    responseObj = json.loads(response.decode(strings.DECODER))
    # verifies that Create Account was successful, reacts accordingly
    utilities.verifyResponse(responseObj)
    print('User "' + username + '" created')
    return User(username, responseObj["key"])


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
    else:
        endpoint += strings.EXPIRY + '-1'
    header = {strings.PASSWORD : password}
    # sends username and password to endpoint site as http "POST" command, receives response
    response = utilities.sendAndReceive(strings.HTTP_GET, endpoint, None, header)
    responseObj = json.loads(response.decode(strings.DECODER))
    # verifies that Login was successful, reacts accordingly
    utilities.verifyResponse(responseObj)
    print('User "' + username + '" logged in')
    return User(username, responseObj["key"])


'''
logout
@param user: User
logs user out of server by destroying API Key
'''
def logout(user):
    endpoint = strings.USER_LOGOUT
    header = {strings.API_KEY : user.getKey()}
    response = utilities.sendAndReceive(strings.HTTP_GET, endpoint, None, header)
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
    def __init__(self, username, apiKey):
        self.username = username
        self.apiKey = apiKey
        self.refresh()
    
    
    def __str__(self):
        myString = "User " + str(self.username) + " with ID " + str(self.userId) + "\n"
        for loc in self.locations:
            myString += "\t+ Location " + str(loc.getName()) +" with ID " + str(loc.getId()) +"\n"
            
            for device in loc.getDevices():
                myString += "\t\t+ Device " + str(device.getDesc()) + " is ID " + str(device.getId()) + " of type " + str(device.getType()) + "\n"
                
                for param in device.getParameters():
                    myString += "\t\t\t+ Parameter " + str(param["name"]) + " at index " + str(param["index"]) + " has value " + str(param["value"]) + "\n"
                
        return myString;

    '''
    refresh
    refreshes User's information from server
    '''
    def refresh(self):
        endpoint = strings.USER
        body = None
        header = {strings.API_KEY : self.apiKey}
        # sends API Key to endpoint site as http "GET" command, receives response
        response = utilities.sendAndReceive(strings.HTTP_GET, endpoint, body, header)
        info = json.loads(response.decode(strings.DECODER))
        # verifies that Login was successful, reacts accordingly
        utilities.verifyResponse(info)
        # extract information about user and cache it in the user object
        userInfo = info["user"]
        self.userId = userInfo["id"]
        # extract information about user's location and cache it in user object
        locDict = info["locations"]
        self.locations = []
        while locDict:
            curLoc = loc.toLoc(self, locDict.pop())
            self.locations.append(curLoc)

    '''
    populateParams
    @return the most recent measurements of the user
    '''
    def populateParams(self):
        endpoint = strings.PARAMS
        body = None
        header = {strings.API_KEY : self.apiKey}
        # sends API Key to endpoint site as http "GET" command, receives response
        response = utilities.sendAndReceive(strings.HTTP_GET, endpoint, body, header)
        responseObj = json.loads(response.decode(strings.DECODER))
        # verifies that Login was successful, reacts accordingly
        utilities.verifyResponse(responseObj)
        return responseObj

    '''
    getDeviceById
    @param deviceId: String
    @return the user's device with the given ID; returns None if ID does not exist
    '''
    def getDeviceById(self, deviceId):
        # go through all user's locations
        for location in self.locations:
            # go through all devices in current location
            for device in location.getDevices():
                # if device has the same device ID as given, return
                if device.getId() == deviceId:
                    return device
        return None

    '''
    getDevicesByProductId
    @param productId: int
    @return a list of the user's devices with this product ID
    '''
    def getDevicesByProductId(self, productId):
        devList = []
        # go through all user's locations
        for location in self.locations:
            # go through all devices in current location
            for device in location.getDevices():
                # if device has the same product ID as given, add it to list of devices
                if device.getType() == productId:
                    devList.append(device)
        return devList

    '''
    getUsername
    @return this User's location
    '''
    def getUsername(self):
        return self.username
    
    '''
    getUserId
    @return this User's ID
    '''
    def getUserId(self):
        return self.userId
    
    '''
    getLoc
    @return this User's location
    '''
    def getLocations(self):
        return self.locations

    '''
    getLocationById
    @param locationId: int
    @return user's location with the given id; return None if doesn't exist
    '''
    def getLocationById(self, locationId):
        for location in self.locations:
            if location.getId() == locationId:
                return location
        return None

    '''
    getKey
    @return the API Key of this User
    '''
    def getKey(self):
        return self.apiKey