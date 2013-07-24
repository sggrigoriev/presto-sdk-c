'''
device
Created on June 25, 2013
@author: Arun Varma
'''
import json
import utilities, strings


'''
register
registers this device with the cloud
@param user: User
@param deviceId: String (case sensitive, without spaces)
@param productId: int 
'''
def register(user, deviceId, productId, desc = None):
    for ch in deviceId:
        if ch == " ":
            raise Exception("Device ID cannot contain spaces")
    loc = user.getLoc()
    endpoint = strings.LOCATIONS + loc.getId().__str__() + strings.LOCATION_DEVICES + deviceId + strings.DEVICE_PRODUCT_ID + productId.__str__()
    header = {strings.API_KEY : user.getKey()}
    # sends product ID and API Key to endpoint site as http "POST" command, receives response
    response = utilities.sendAndReceive(strings.POST, endpoint, None, header)
    responseObj = json.loads(response.decode(strings.DECODER))
    # verifies that register device was successful, reacts accordingly
    utilities.verifyResponse(responseObj)
    print('Device with ID "' + deviceId + '" registered')
    return Device(user, deviceId, loc, desc)


class DeviceVitals(object):
    '''
    __init__
    defines a device object with a unique deviceId, a user, a description and a location
    @param deviceId: String (case sensitive with no spaces)
    @param user: User
    @param desc: String (user description of the device)
    @param loc: Location
    '''
    def __init__(self, deviceId = None, loc = None, desc = None):
        self.id = deviceId
        self.location = loc
        self.desc = desc

class Device(object):
    '''
    __init__
    defines a device object with a unique deviceId, a user, a description and a location
    @param deviceId: String (case sensitive with no spaces)
    @param user: User
    @param desc: String (user description of the device)
    @param loc: Location
    '''
    def __init__(self, user, deviceId, loc, desc = None):
        # verifies that device ID only contains valid characters
        for char in deviceId:
            if char == " ":
                raise Exception("Device ID cannot contain spaces")
        self.user = user
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
    nickname
    sets the device's description to given desc
    @param desc: String
    '''
    def nickname(self, nickname):
        forJson = DeviceVitals(desc = nickname)
        endpoint = strings.DEVICES + self.id
        header = {strings.CONTENT_TYPE : strings.JSON_APPLICATION, strings.API_KEY : self.user.getKey()}
        body = '{"device": ' + utilities.toJson(forJson) + "}"
        # sends product ID and API Key to endpoint site as http "POST" command, receives response
        response = utilities.sendAndReceive(strings.PUT, endpoint, body, header)
        utilities.verifyResponse(json.loads(response.decode(strings.DECODER)))
        self.desc = nickname
        print("Device description updated: " + nickname)

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
    returns the description (nickname) of this device
    '''
    def getDesc(self):
        return self.desc

    '''
    getParams
    gets specified parameters of this device
    if params is not specified, will return the last known parameters
    @param params: Parameter
    '''
    def getParams(self, params = None):
        return
