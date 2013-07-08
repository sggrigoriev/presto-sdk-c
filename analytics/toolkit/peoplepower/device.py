'''
Created on June 25, 2013
@author: Arun Varma
'''
import toolkit
import urllib.parse as urllib


'''
register
registers this device with the cloud
@param aUser: User
@param deviceId: String
@param productId: int
@param desc: String (user description of the device)
'''
def register(aUser, deviceId, productId, desc):
    apiKey = aUser.getApiKey()
    loc = aUser.getLocs().pop()
    endpoint = "/cloud/json/deviceRegistration/" + (loc.getId()).__str__() + "/" + deviceId
    body = urllib.urlencode({"productId" : productId})
    header = {"PRESENCE_API_KEY" : apiKey}
    # send product ID and API Key to endpoint site as http "POST" command, receive response
    toolkit.sendAndReceive("POST", endpoint, body, header)
    print("Device registered with ID " + deviceId)
    return Device(aUser, deviceId, loc, desc)


'''
getParams
gets specified parameters of this device
if params is not specified, will return the last known parameters
@param params: Parameter
'''
def getParams(self, deviceId = None, params = None, index = None):
    endpoint = "/cloud/json/parameters"
    body = {}
    header = {"PRESENCE_API_KEY" : self.apiKey}
    toolkit.sendAndReceive("GET", endpoint, body, header)
    print(self.username + " refreshed")


class Device(object):
    '''
    __init__
    defines a device object with a unique deviceId, a user, a description and a location
    @param deviceId: String (case sensitive with no spaces)
    @param user: User
    @param desc: String (user description of the device)
    @param loc: Location
    '''
    def __init__(self, aUser, deviceId, loc, desc):
        self.user = aUser
        self.id = deviceId
        self.loc = loc
        self.desc = desc

    '''
    refreshDevicesFromServer
    ?????????????????????????
    '''
    def refreshDevicesFromServer(self):
        return

    '''
    populate
    populates specified parameters of this device with current information
    if params is not specified, the last known parameters will be populated
    @param params: Parameter[]
    '''
    def populate(self, params = None):
        if params == None:
            params = self.getParams()

    '''
    getUser
    returns the user of this device
    '''
    def getUser(self):
        return self.user

    '''
    getDeviceId
    returns the ID number of this device
    '''
    def getId(self):
        return self.id

    '''
    getLoc
    returns the location of this device
    '''
    def getLoc(self):
        return self.loc

    '''
    getDesc
    returns the description of this device
    '''
    def getDesc(self):
        return self.desc