'''
Created on Aug 14, 2013
@author: Arun Varma
'''


ON = "ON"
OUTLET_STATUS = "outletStatus"
DEFAULT_PRODUCT_ID = 2012


'''
run
turns all the user's default devices on
'''
def run(user):
    # turn on all devices with the default product id
    devices = user.getDevicesByProductId(DEFAULT_PRODUCT_ID)
    for device in devices:
        turnOnOutletStatus(device)


'''
turnOnOutletStatus
turn device's outletStatus on
'''
def turnOnOutletStatus(device):
    device.sendCommand(OUTLET_STATUS, None, ON)