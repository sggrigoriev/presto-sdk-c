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
    locId = utilities.setVal("id", locDict)
    name = utilities.setVal("name", locDict)
    timezone = utilities.setVal("timezone", locDict)
    addrStreet1 = utilities.setVal("addrStreet1", locDict)
    addrStreet2 = utilities.setVal("addrStreet2", locDict)
    city = utilities.setVal("city", locDict)
    state = utilities.setVal("state", locDict)
    country = utilities.setVal("country", locDict)
    zipcode = utilities.setVal("zipcode", locDict)
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
