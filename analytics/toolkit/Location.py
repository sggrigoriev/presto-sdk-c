'''
Created on June 25, 2013
@author: Arun Varma
'''

class Location(object):
    '''
    __init__
    defines a Location object
    '''
    def __init__(self, name, user, locationId, stateId, countryId, countryLabel, timezone, zipcode):
        self._name = name
        self._user = user
        self.locationId = locationId
        self._stateId = stateId
        self._countryId = countryId
        self._countryLabel = countryLabel
        self._timezone = timezone
        self._zipcode = zipcode
        self._devices = []

    '''
    getName
    returns the name of this Location
    '''
    def getName(self):
        return self._name

    '''
    getUser
    returns the ?????????????????????????????????
    '''
    def getUser(self):
        return self._user

    '''
    getLocation
    returns the ID number of this Location
    '''
    def getLocation(self):
        return self._locationId

    '''
    getState
    returns the stateId of this Location
    '''
    def getState(self):
        return self._stateId

    '''
    getCountryId
    returns the countryId of this Location
    '''
    def getCountryId(self):
        return self._countryId

    '''
    getCountryLabel
    returns the countryLabel of this Location
    '''
    def getCountryLabel(self):
        return self._countryLabel

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