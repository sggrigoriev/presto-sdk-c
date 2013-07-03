'''
Created on June 27, 2013
@author: Arun Varma
'''
import user
from location import Location
from device import Device

if __name__ == '__main__':
    loc = Location("Home")
    user.createAccount("arun_varma+19@peoplepowerco.com", "password", "FirstApp", "arun_varma+19@peoplepowerco.com", loc)
    '''
    user.login("arun_varma+13@peoplepowerco.com", "password")
    '''