'''
utilities
Created on July 14, 2013
@author: Arun Varma
'''
import http.client as http, json
import strings


# result code for successful action
SUCCESS = 0


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
    conn = http.HTTPSConnection(strings.URL_ROOT, 443)
    conn.connect()
    # sends body and header via an HTTP request command to endpoint site
    conn.request(request, endpoint, body, header)
    # gets response from People Power server, decodes information
    response = conn.getresponse().read()
    return response

'''
verifyResponse
verifies that request was successful, raises an exception if not
@param responseObj: dict
'''
def verifyResponse(responseObj):
    # get result code from dictionary
    resultCode = responseObj["resultCode"]
    # if action was not successful, throw an error corresponding to result code
    if resultCode != SUCCESS:
        raise Exception("(" + resultCode.__str__() + ") " + responseObj["resultCodeDesc"] + ": " + responseObj["resultCodeMessage"])

'''
setVal
returns the corresponding value to key in dictionary; returns None if not found in dictionary
@param key: String
@param dictionary: dictionary
'''
def setVal(key, dictionary):
    # if key is found in locDict, return corresponding value
    if key in dictionary:
        return dictionary[key]
    else:
        return None