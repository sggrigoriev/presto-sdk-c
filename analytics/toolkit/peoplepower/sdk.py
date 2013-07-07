'''
Created on June 25, 2013
@author: Arun Varma
'''
import json, http.client as http


# root URL connection
urlRoot = "alpha2.peoplepowerco.com"
# result code for successful action
SUCCESS = 0

'''
toJson
returns a JSON representation of the given object
'''
def toJson(obj):
    # create a dictionary of object's attributes; disregards attributes with a value of null
    attrDict = dict([(attr, obj.__dict__[attr]) for attr in obj.__dict__ if obj.__dict__[attr]])
    # return a JSON representation of this dictionary
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
    # send body and header via an HTTP request command to endpoint site
    conn.request(request, endpoint, body, header)
    # get response from People Power server, decode information
    response = conn.getresponse().read()
    return response


'''
errorCode
raises an Exception corresponding to the given result code
@param resultCode: int
'''
def verifyResponse(responseObj):
    resultCode = responseObj["resultCode"]
    if resultCode != SUCCESS:
        errorDesc = responseObj["resultCodeDesc"]
        errorMessage = responseObj["resultCodeMessage"]
        raise Exception("(" + resultCode.__str__() + ") " + errorDesc + ": " + errorMessage)


'''
getProductId
returns the Product ID of the given device
@param deviceId: int
'''
def getProductId(deviceId):
    return


'''
user
??????????????????????????????????????????
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
??????????????????????????????????????????
@param apiKey: String
@param loc: Location
'''
def devicesInfo(apiKey, loc):
    return


'''
run
??????????????????????????????????????????
'''
def run():
    return