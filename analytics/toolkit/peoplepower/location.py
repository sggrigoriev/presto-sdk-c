'''
Created on June 25, 2013
@author: Arun Varma
'''


def isThere(item, locDict):
    if locDict.__contains__(item):
        return locDict[item]
    else:
        return None


def toLoc(user, locDict):
    name = locDict["name"]
    idNo = locDict["id"]
    away = isThere("away", locDict)
    address1 = isThere("addrstreet1", locDict)
    address2 = isThere("addrstreet2", locDict)
    city = isThere("city", locDict)
    state = isThere("state", locDict)
    country = isThere("country", locDict)
    zipcode = isThere("zip", locDict)
    timezone = isThere("timezone", locDict)
    return Location(user, name, idNo, away, address1, address2, city, state, country, zipcode, timezone)


class LocationVitals(object):
    '''
    __init__
    defines a Location object with only attributes necessary to be serialized as a JSON object
    @param name: String
    @param state: State
    @param country: Country
    @param city: String
    @param timezone: Timezone
    @param zipcode: String
    '''
    def __init__(self, name, address1 = None, address2 = None, city = None, state = None, country = None, zipcode = None, timezone = None):
        self.name = name
        self.addrstreet1 = address1
        self.addrstreet2 = address2
        self.addrcity = city
        self.state = state
        self.country = country
        self.zip = zipcode
        self.timezone = timezone


class Location(object):
    '''
    __init__
    defines a Location object
    @param user: User
    @param name: String
    @param away: boolean
    @param idNo: int
    @param address1: String
    @param address2: String
    @param city: String
    @param state: State
    @param country: Country
    @param zipcode: String
    @param timezone: Timezone
    '''
    def __init__(self, user, name, away, idNo, address1 = None, address2 = None, city = None, state = None, country = None, zipcode = None, timezone = None):
        self.user = user
        self.id = idNo
        self.away = away
        self.name = name
        self.timezone = timezone
        self.addrstreet1 = address1
        self.addrstreet2 = address2
        self.addrcity = city
        self.state = state
        self.country = country
        self.zip = zipcode


    '''
    refreshFromServer
    ????????????????????????????//
    '''
    def refreshFromServer(self):
        return

    '''
    addDevice
    adds the given device to this Location's list of devices
    @param device: Device
    '''
    def addDevice(self, device):
        self.devices.add(device)

    '''
    getDevices
    @return the list of Devices at this Location
    '''
    def getDevices(self):
        return self.devices

    '''
    getUser
    @return the user at this Location
    '''
    def getUser(self):
        return