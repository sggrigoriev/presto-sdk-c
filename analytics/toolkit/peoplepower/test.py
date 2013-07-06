'''
Created on June 27, 2013
@author: Arun Varma
'''
import user
from location import Location

if __name__ == '__main__':
    loc = Location("Home")
    '''
    user.createAccount("arun_varma+23@peoplepowerco.com", "password", "FirstApp", "arun_varma+23@peoplepowerco.com", loc)
    '''
    '''
    user.login("arun_varma+13@peoplepowerco.com", "password")
    '''
    us1 = user.createAccount("arun_varma+28@peoplepowerco.com", "password", "FirstApp", "arun_varma+28@peoplepowerco.com", loc)
    print(us1.getAllInfo())
    print(us1.getLocationsInfo())
    print(us1.getUserInfo())
    us1.logout()