'''
device
Created on June 25, 2013
@author: Arun Varma
'''
import json
import peoplepower.utilities as utilities
import peoplepower.strings as strings

'''
toDevice
converts devDict to a device object
@param user: User
@param devDict: dictionary containing the properties of a device
'''
def toDevice(loc, devDict):
    # if values are found in devDict, store them
    deviceId = utilities.setVal("id", devDict)
    productId = utilities.setVal("type", devDict)
    # return device object with these values
    return Device(loc, deviceId, productId)


'''
register
registers this device with the cloud
@param loc: Location
@param deviceId: String (case sensitive, without spaces)
@param productId: int 
'''
def register(loc, deviceId, productId):
    for ch in deviceId:
        if ch == " ":
            raise Exception("Device ID cannot contain spaces")
    endpoint = strings.LOCATIONS + loc.getId().__str__() + strings.LOCATION_DEVICES + deviceId + strings.DEVICE_PRODUCT_ID + productId.__str__()
    body = None
    header = {strings.API_KEY : loc.getUser().getKey()}
    # sends product ID and API Key to endpoint site as http "POST" command, receives response
    response = utilities.sendAndReceive(strings.POST, endpoint, body, header)
    responseObj = json.loads(response.decode(strings.DECODER))
    # verifies that register device was successful, reacts accordingly
    utilities.verifyResponse(responseObj)
    print('Device with ID "' + deviceId + '" registered')
    toReturn = Device(loc, deviceId, productId)
    loc.addDevice(toReturn)
    return toReturn


class DeviceVitals(object):
    '''
    __init__
    defines a device object with a unique deviceId, a user, a description and a location
    @param deviceId: String (case sensitive with no spaces)
    @param productId: int
    @param desc: String (user description of the device)
    @param loc: Location
    '''
    def __init__(self, deviceId = None, productId = None, loc = None, desc = None):
        self.id = deviceId
        self.location = loc
        self.productId = productId
        self.desc = desc

class Device(object):
    '''
    __init__
    defines a device object with a unique deviceId, a user, a description and a location
    @param user: User
    @param deviceId: String (case sensitive with no spaces)
    @param productId: int
    @param desc: String (user description of the device)
    '''
    def __init__(self, loc, deviceId, productId):
        self.loc = loc
        self.id = deviceId
        self.type = productId
        self.desc = None
        self.refresh()

    '''
    refreshFromServer
    refreshes Device's information from server
    '''
    def refresh(self):
        endpoint = strings.DEVICES + self.id
        body = None
        header = {strings.API_KEY : self.loc.getUser().getKey()}
        # sends device ID and API Key to endpoint site as http "GET" command, receives response
        response = utilities.sendAndReceive(strings.GET, endpoint, body, header)
        info = json.loads(response.decode(strings.DECODER))
        # verifies that register device was successful, reacts accordingly
        utilities.verifyResponse(info)
        
        # extract device information and update device properties correspondingly
        devInfo = info["device"]
        self.id = devInfo["id"]
        self.type = devInfo["type"]
        self.desc = utilities.setVal("desc", devInfo)

    '''
    populateParams
    populates specified parameters of this device with current information
    if params is not specified, the last known parameters will be populated
    @param params: String[]
    '''
    def populateParams(self, params = None):
        endpoint = strings.PARAMS_DEVICE + self.id
        if params != None:
            while params:
                endpoint += strings.PARAM_NAME + params.pop()
        print(endpoint)
        header = {strings.API_KEY : self.loc.getUser().getKey()}
        body = None
        # sends device ID and API Key to endpoint site as http "GET" command, receives response
        response = utilities.sendAndReceive(strings.GET, endpoint, body, header)
        responseObj = json.loads(response.decode(strings.DECODER))
        # verifies that register device was successful, reacts accordingly
        utilities.verifyResponse(responseObj)
        return responseObj

    '''
    nickname
    sets the device's description to given desc
    @param desc: String
    '''
    def nickname(self, nickname):
        forJson = DeviceVitals(desc = nickname)
        endpoint = strings.DEVICES + self.id
        header = {strings.CONTENT_TYPE : strings.JSON_APPLICATION, strings.API_KEY : self.loc.getUser().getKey()}
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
    getType
    returns the type (product ID) of this device
    '''
    def getType(self):
        return self.type

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