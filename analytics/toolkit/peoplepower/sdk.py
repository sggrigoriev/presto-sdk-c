import json

'''
Created on June 25, 2013
@author: Arun Varma
'''

class Sdk:
    '''
    rootSite
    @return the People Power root site
    '''
    @staticmethod
    def rootSite():
        return "alpha2.peoplepowerco.com"

    '''
    user
    
    @param apiKey: String
    '''
    def user(self, apiKey):
        return

    '''
    registerNew
    registers the given Product ID with the cloud
    @param productId: int
    '''
    @staticmethod
    def registerNew(productId):
        return

    '''
    devicesInfo
    
    @param apiKey: String
    @param loc: Location
    '''
    @staticmethod
    def devicesInfo(apiKey, loc):
        return

    '''
    getProductId
    returns the Product ID of the given device
    @param deviceId: int
    '''
    @staticmethod
    def getProductId(deviceId):
        return

    '''
    toJson
    returns a JSON representation of the given object
    '''
    @staticmethod
    def toJson(this):
        return json.dumps(this, default=lambda obj: obj.__dict__, indent=2)

    '''
    run
    
    '''
    def run(self):
        return