from sdk import Sdk
import http.client as http, urllib

'''
Created on June 25, 2013
@author: Arun Varma
'''

class Device(object):
    '''
    __init__
    defines a device object with a unique deviceId and a location
    @param deviceId: String (case sensitive with no spaces)
    @param loc: Location
    '''
    def __init__(self, user, deviceId, loc):
        for char in deviceId:
            if char == " ":
                raise Exception("Device ID cannot have spaces")
        self._user = user
        self._deviceId = deviceId
        self._loc = loc
        '''
        self._conn = http.client("https://developer.peoplepowerco.com")
        '''

    '''
    getDeviceId
    returns the ID number of this device
    '''
    def getDeviceId(self):
        return self._deviceId

    '''
    getLocation
    returns the location of this device
    '''
    def getLocation(self):
        return self._loc

    '''
    register
    registers this device with the cloud
    '''
    def register(self):
        params = {}
        params["PRESENCE_API_KEY"] = self._user.getKey()
        params["productId"] = Sdk.getProductId(self._deviceId)
        urllib.request.urlopen(Sdk.getRootSite() + "/cloud/xml/deviceRegistration/", urllib.parse.urlencode(params))

    '''
    getParams
    gets specified parameters of this device
    if params is not specified, will return the last known parameters
    @param params: Parameter
    '''
    def getParams(self, params = None):
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