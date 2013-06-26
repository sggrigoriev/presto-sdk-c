'''
Created on June 25, 2013
@author: Arun Varma
'''

class User(object):
    '''
    __init__
    defines a User object with a unique userId and list of locations where they have put devices
    @param userId: int
    @param locations: Location[]
    '''
    def __init__(self, userId):
        self._userId = userId
        self._locations = []

    '''
    getId
    returns the ID number of this user
    '''
    def getId(self):
        return self._userId

    '''
    getLocations
    returns this user's list of locations
    '''
    def getLocations(self):
        return self._locations

    '''
    addLocation
    adds a location to this User's list of locations
    @param loc: Location
    '''
    def addLocation(self, loc):
        self._locations.add(loc)

    '''
    login
    returns the user's API Key
    @param username: String
    @param password: String
    '''
    def login(self, username, password):
        return