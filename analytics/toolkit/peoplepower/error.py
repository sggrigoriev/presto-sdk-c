'''
Error
Created on August 2, 2013
@author: David Moss
'''


'''
API Exception
@param resultCode: the result code given to us by the API
@param description: the description from the API error
@param message: the message from the API error
'''
class ApiError(Exception):
    def __init__(self, resultCode, description, message):
        self.resultCode = resultCode
        self.description = description
        self.message = message
        
    def __str__(self):
        return self.message
