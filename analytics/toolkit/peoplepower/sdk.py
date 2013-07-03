'''
Created on June 25, 2013
@author: Arun Varma
'''
import json, http.client as http


# root URL connection
urlRoot = "alpha2.peoplepowerco.com"
# result code for successful action
SUCCESS = 0
# dictionary with keys as result codes and values as corresponding messages
resultCodeDict = {0 : "Success", 1 : "Internal error", 2 :"Wrong API key", 3 : "Wrong location ID or the location is not related to the user",
    4 : "Wrong device ID or device has not been found", 5 : "Device command proxy (gateway) has not been found", 6 : "Object not found",
    7 : "Access denied", 8 : "Wrong parameter value", 9 : "Missed mandatory parameter value", 10 : "No available device resources to complete operation",
    11 : "Wrong username", 12 : "Invalid username or wrong password", 13 : "Wrong index value", 14 : "Error in parsing of input data",
    15 : "Wrong consumer ID", 16 : "Wrong consumer type ID", 17 : "Wrong activation key", 18 : "Wrong rule schedule format", 19 : "Wrong rule ID",
    20 : "Duplicate user name", 21 : "Device is offline or disconnected", 22 : "Device is under different location", 23 : "Analytic rule generation error",
    24 : "Wrong device registration code", 25 : "Number of pending product ID's for this user is more than allowed", 26 : "Duplicate entity or property",
    27 : "Timeout in device communication", 28 : "Device is not linked to any location", 29 : "Requested API method not available",
    30 : "Service is temporarily unavailable"}

'''
toJson
returns a JSON representation of the given object
'''
def toJson(obj):
    # create a dictionary of object's attributes; disregards attributes with a value of null
    attrDict = dict([(attr, obj.__dict__[attr]) for attr in obj.__dict__ if obj.__dict__[attr]])
    # returns a JSON representation of this dictionary
    return json.dumps(attrDict, default=lambda o: o.__dict__, indent=2)


'''
sendAndReceive
connects to root URL, sends body and header to endpoint site an HTTP command corresponding to request
@param request: String
@param endpoint: String
@param body: String
@param header: String
'''
def sendAndReceive(request, endpoint, body, header):
    # connect to root URL
    conn = http.HTTPSConnection(urlRoot, 443)
    conn.connect()
    # sends body and header via an HTTP request command to endpoint site
    conn.request(request, endpoint, body, header)
    # gets response from People Power server, decodes information
    response = conn.getresponse().read()
    return response


'''
errorCode
raises an Exception corresponding to the given result code
@param resultCode: int
'''
def errorCode(resultCode):
    raise Exception("(Error code " + resultCode.__str__() + ") " + resultCodeDict[resultCode])


'''
getProductId
returns the Product ID of the given device
@param deviceId: int
'''
def getProductId(deviceId):
    return


'''
user

@param apiKey: String
'''
def user(apiKey):
    return


'''
registerNew
registers the given Product ID with the cloud
@param productId: int
'''
def registerNewProduct(productId):
    return


'''
devicesInfo

@param apiKey: String
@param loc: Location
'''
def devicesInfo(apiKey, loc):
    return


'''
run

'''
def run():
    return