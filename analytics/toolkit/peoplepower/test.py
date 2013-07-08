'''
Created on June 27, 2013
@author: Arun Varma
'''
import user, device, timezone
from location import LocationVitals

if __name__ == '__main__':
    loc = LocationVitals("Home")
    '''
    user.createAccount("arun_varma+23@peoplepowerco.com", "password", "FirstApp", "arun_varma+23@peoplepowerco.com", loc)
    '''
    user.login("example@peoplepowerco.com", "password")
    '''
    us1 = user.createAccount("arun_varma+92@peoplepowerco.com", "password", "FirstApp", "arun_varma+92@peoplepowerco.com", loc)
    dev = device.register(us1, "thedeviceid", 3, "this is a new device")
    '''