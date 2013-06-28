import json

'''
Created on June 25, 2013
@author: Arun Varma
'''

class Location(object):
    '''
    __init__
    defines a Location object
    @param name: String
    @param stateId: int
    @param countryId: int
    @param city: String
    @param timezone: String
    @param zipcode: String
    '''
    def __init__(self, name, city = None, stateId = None, countryId = None, timezone = None, zipcode = None):
        self.name = name
        self.stateId = stateId
        self.countryId = countryId
        self.timezone = timezone
        self.zip = zipcode

    '''
    getUser
    returns the user at this Location
    '''
    def getUser(self):
        return self._user

    '''
    getName
    returns the name of this Location
    '''
    def getName(self):
        return self._name

    '''
    getCity
    returns the city of this Location
    '''
    def getCity(self):
        return self._city

    '''
    getStateId
    returns the state ID of this Location
    '''
    def getStateId(self):
        return self._stateId

    '''
    getCountryId
    returns the country ID of this Location
    '''
    def getCountryId(self):
        return self._countryId

    '''
    getTimezone
    returns the timezone of this Location
    '''
    def getTimezone(self):
        return self._timezone

    '''
    getZipcode
    returns the zipcode of this Location
    '''
    def getZipcode(self):
        return self._zipcode

    '''
    getDevices
    returns the list of Devices at this Location
    '''
    def getDevices(self):
        return self._devices

    '''
    addDevice
    adds the given device to this Location's list of devices
    '''
    def addDevice(self, device):
        self._devices.add(device)

    '''
    toJSON
    returns the JSON representation of this Location
    '''
    def toJSON(self):
        newDict = dict([(attr,self.__dict__[attr]) for attr in self.__dict__ if self.__dict__[attr]])
        return json.dumps(newDict, default=lambda obj: obj.__dict__, indent=2)