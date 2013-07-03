'''
Created on June 25, 2013
@author: Arun Varma
'''
import json, sdk
from user import CachedUser
import urllib


'''
register
registers this device with the cloud
@param deviceId: String
@param productId: int
@param user: User
'''
def register(deviceId, productId, user):
    # a JSON object with all known details about user and associated Locations
    jsonUser = user.getInfo()
    # decode response from unicode to String, deserialize JSON code into a Python dictionary
    responseObj = json.loads(jsonUser.decode('utf-8'))
    endpoint = "/cloud/xml/deviceRegistration/" + responseObj[""] + "/" + deviceId
    body = {"productId" : sdk.getProductId(productId)}
    header = {"PRESENCE_API_KEY" : user.getApiKey()}
    # sends product ID and API Key to endpoint site as http "POST" command, receives response
    sdk.sendAndReceive("POST", endpoint, body, header)


class Device(object):
    '''
    __init__
    defines a device object with a unique deviceId, a user, a description and a location
    @param deviceId: String (case sensitive with no spaces)
    @param user: User
    @param desc: String (user description of the device)
    @param loc: Location
    '''
    def __init__(self, deviceId, user, desc, loc):
        # verifies that device ID only contains valid characters
        for char in deviceId:
            if char == " ":
                raise Exception("Device ID can only take valid characters")
        self.id = deviceId
        self.user = user
        self.desc = desc
        self.location = loc

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
    getParams
    gets specified parameters of this device
    if params is not specified, will return the last known parameters
    @param params: Parameter
    '''
    def getParams(self, params = None):
        return

    '''
    getDeviceId
    returns the ID number of this device
    '''
    def getId(self):
        return self.id

    '''
    getLocation
    returns the location of this device
    '''
    def getLocation(self):
        return self.loc