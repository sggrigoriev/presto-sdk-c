'''
test
Created on June 27, 2013
@author: Arun Varma
'''


from loc import LocVitals
import user
import device


if __name__ == '__main__':
    '''
    loc
    '''
    loc = LocVitals("Living Room")


    '''
    user
    '''
    #createAccount
    us1 = user.createAccount("testing+14@peoplepowerco.com", "password", "FirstApp", "testing+14@peoplepowerco.com", loc) #successful
    try:
        user.createAccount("testing@peoplepowerco.com", "password", "FirstApp", "testing@peoplepowerco.com", loc) #duplicate account
    except:
        print("Duplicate user")
    try:
        user.createAccount("testingpeoplepowerco.com", "password", "FirstApp", "testingpeoplepowerco.com", loc) #invalid email
    except:
        print("Invalid email")
    #login
    us2 = user.login("testing@peoplepowerco.com", "password") #successful
    try:
        user.login("testing@peoplepowerco.com", "pass") #invalid password
    except:
        print("Invalid password")
    try:
        user.login("this@doesntexist.com", "password") #username nonexistent
    except:
        print("Username nonexistent")
    #getLoc
    print(us1.getLoc())
    print(us2.getLoc())
    #getKey
    print("User1 key: " + us1.getKey())
    print("User2 key: " + us2.getKey())
    #logout
    user.logout(us2)
    print(us2.getKey())


    '''
    device
    '''
    #register
    dev1 = device.register(us1, "id-17", 3) #successful
    dev2 = device.register(us1, "id-18", 3, "new device") #successful - device with description
    try:
        device.register(us1, "an id", 3) #invalid device id
    except:
        print("Invalid device id")
    #getDesc
    print(dev1.getDesc() == None)
    print(dev2.getDesc() == "new device")
    #nickname
    dev1.nickname("main device") #give description of device
    dev2.nickname("secondary device") #change description of device
    print(dev1.getDesc() == "main device")
    print(dev2.getDesc() == "secondary device")
    #getId
    print(dev1.getId())
    print(dev2.getId())
