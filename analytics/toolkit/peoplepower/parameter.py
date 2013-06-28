'''
Created on June 25, 2013
@author: Arun Varma
'''

class Parameter(object):
    '''
    __init__
    defines a Parameter object with a name, index, multiplier, and value
    @param name: String
    @param index: String (optional - will be set to None if not specified)
    @param multiplier: String or enum
    @param value: String or int
    '''
    def __init__(self, name, multiplier, value, index = None):
        self._name = name
        self._index = index
        self._multiplier = multiplier
        self._value = value

    '''
    getName
    returns the name of this parameter
    '''
    def getName(self):
        return self._name

    '''
    getIndex
    returns the index of this parameter
    '''
    def getIndex(self):
        return self._index

    '''
    getMult
    returns the multiplier of this parameter
    '''
    def getMult(self):
        return self._multiplier

    '''
    getVal
    returns the value of this parameter
    '''
    def getVal(self):
        return self._value

    '''
    setVal
    resets the value of the this parameter to the given newVal
    @param newVal: String or int
    '''
    def setVal(self, newVal):
        self._value = newVal