'''
Created on June 25, 2013
@author: Arun Varma
'''
import sdk


'''
register
registers this device with the cloud
@param aUser: User
@param deviceId: String
@param productId: int
@param desc: String (user description of the device)
'''
def register(loc, apiKey, deviceId, productId, desc):
    locId = loc.
    endpoint = "/cloud/json/deviceRegistration/" + locId + "/" + deviceId
    body = {"productId" : productId}
    header = {"PRESENCE_API_KEY" : apiKey}
    # send product ID and API Key to endpoint site as http "POST" command, receive response
    sdk.sendAndReceive("POST", endpoint, body, header)
    return Device(apiKey, deviceId, desc, locId)


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
    sdk.sendAndReceive("GET", endpoint, body, header)
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
        # verify that device ID only contains valid characters
        for char in deviceId:
            if char == " ":
                raise Exception("Device ID can only take valid characters")
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