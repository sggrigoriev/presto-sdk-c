'''
Created on June 27, 2013
@author: Arun Varma
'''
import user, sdk
from location import Location, LocationVitals

if __name__ == '__main__':
    loc = LocationVitals("Home")
    '''
    user.createAccount("arun_varma+23@peoplepowerco.com", "password", "FirstApp", "arun_varma+23@peoplepowerco.com", loc)
    '''
    '''
    user.login("arun_varma+13@peoplepowerco.com", "password")
    '''
    us1 = user.createAccount("arun_varma+56@peoplepowerco.com", "password", "FirstApp", "arun_varma+56@peoplepowerco.com", loc)
    us1.logout()