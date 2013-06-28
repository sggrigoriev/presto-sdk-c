from location import Location
from user import User
from device import Device

'''
Created on Jun 27, 2013
@author: Arun Varma
'''

if __name__ == '__main__':
    loc = Location("id")
    print(loc.toJSON())
    user1 = User("asdfas")
    print(user1.toJSON())
    dev = Device(user1, "adf", loc)
    user1.createAccount("asdfsad", "asdfasd", "asdfds", loc, "Arun", "Varma")