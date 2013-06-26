'''
Created on June 25, 2013
@author: Arun Varma
'''

class Device(object):
    '''
    __init__
    defines a device object with a unique deviceId and a location
    @param deviceId: int
    @param loc: Location
    '''
    def __init__(self, deviceId, loc):
        self._deviceId = deviceId
        self._loc = loc

    '''
    getId
    returns the ID number of this device
    '''
    def getId(self):
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
        return

    '''
    getParams
    gets the last known parameters of this device
    '''
    def getParams(self):
        return

    '''
    populate
    populates last known parameters of this device with current information
    @param params: Parameter[]
    '''
    def populate(self, params):
        return