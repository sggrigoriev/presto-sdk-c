'''
loc
Created on June 25, 2013
@author: Arun Varma
'''
import peoplepower.utilities as utilities
import peoplepower.strings as strings
import peoplepower.device as device
import json


'''
toLoc
converts locDict to a location object
@param user: User
@param locDict: dictionary containing the properties of a location
'''
def toLoc(user, locDict):
    # if values are found in locDict, store them
    locId = locDict.get("id", None)
    name = locDict.get("name", None)
    timezone = locDict.get("timezone", None)
    addrStreet1 = locDict.get("addrStreet1", None)
    addrStreet2 = locDict.get("addrStreet2", None)
    city = locDict.get("city", None)
    state = locDict.get("state", None)
    country = locDict.get("country", None)
    zipcode = locDict.get("zipcode", None)
    # return location object with these values
    return Loc(user, locId, name, timezone, addrStreet1, addrStreet2, city, state, country, zipcode)

class LocVitals(object):
    '''
    __init__
    defines a Location object with only attributes necessary to be serialized as a JSON object
    @param name: String
    @param timezone: Timezone
    @param address1: String
    @param address2: String
    @param city: String
    @param state: State
    @param country: Country
    @param zipcode: String
    '''
    def __init__(self, name, timezone = None, address1 = None, address2 = None, city = None, state = None, country = None, zipcode = None):
        self.name = name
        self.timezone = timezone
        self.addrStreet1 = address1
        self.addrStreet2 = address2
        self.addrCity = city
        self.state = state
        self.country = country
        self.zip = zipcode


class Loc(object):
    '''
    __init__
    defines a Location object
    @param user: User
    @param locId: int
    @param name: String
    @param state: State
    @param country: Country
    @param city: String
    @param timezone: Timezone
    @param zipcode: String
    '''
    def __init__(self, user, locId, name, timezone = None, address1 = None, address2 = None, city = None, state = None, country = None, zipcode = None):
        self.user = user
        self.id = locId
        self.name = name
        self.timezone = timezone
        self.addrstreet1 = address1
        self.addrstreet2 = address2
        self.addrcity = city
        self.state = state
        self.country = country
        self.zip = zipcode
        self.refresh()

    '''
    refreshDevices
    refreshes all of User's devices from server
    '''
    def refresh(self):
        endpoint = strings.DEVICES
        body = None
        header = {strings.API_KEY : self.user.getKey()}
        # sends API Key to endpoint site as http "GET" command, receives response
        response = utilities.sendAndReceive(strings.GET, endpoint, body, header)
        responseObj = json.loads(response.decode(strings.DECODER))
        # verifies that Login was successful, reacts accordingly
        utilities.verifyResponse(responseObj)
        devInfo = responseObj["devices"]
        # extract information about user's devices and cache it in user object
        self.devices = []
        while devInfo:
            curDev = device.toDevice(self, devInfo.pop())
            self.devices.append(curDev)
        
        self.refreshAllDeviceParameters()
            
    def refreshAllDeviceParameters(self):
        endpoint = strings.PARAMS
        body = None
        header = {strings.API_KEY : self.user.getKey()}
        # sends API Key to endpoint site as http "GET" command, receives response
        response = utilities.sendAndReceive(strings.GET, endpoint, body, header)
        responseObj = json.loads(response.decode(strings.DECODER))
        # verifies that Login was successful, reacts accordingly
        utilities.verifyResponse(responseObj)
        print(response.decode(strings.DECODER))
        
        devInfo = responseObj["devices"]
        while devInfo:
            focusedDevInfo = devInfo.pop()
            device = self.getDeviceById(focusedDevInfo["id"])
            paramInfo = focusedDevInfo["parameters"]
            while paramInfo:
                focusedParamInfo = paramInfo.pop()
                
                device.setParameter(focusedParamInfo.get("name", None), 
                                    focusedParamInfo.get("index", None),
                                    focusedParamInfo.get("units", None), 
                                    focusedParamInfo.get("multiplier", None),
                                    focusedParamInfo.get("value", None),
                                    focusedParamInfo.get("lastUpdateTime", None))
    
    '''
    addDevice
    adds the given device to this Location's list of devices
    @param device: Device
    '''
    def addDevice(self, device):
        self.devices.add(device)

    '''
    getDevices
    returns a list of devices belonging to the user
    '''
    def getDevices(self):
        return self.devices
    
    '''
    getUser
    @return the user at this Location
    '''
    def getUser(self):
        return self.user

    '''
    getId
    @return the ID of this Location
    '''
    def getId(self):
        return self.id

    '''
    getName
    @return the name of this Location
    '''
    def getName(self):
        return self.name
    
    '''
    getDeviceById
    @return the Device if it exists
    '''
    def getDeviceById(self, deviceId):
        for device in self.devices:
            if(device.getId() == deviceId):
                return device
        
        return None
    

