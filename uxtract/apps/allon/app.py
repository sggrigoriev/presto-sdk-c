'''
Created on Aug 14, 2013
@author: Arun Varma
'''


OFF = "OFF"
OUTLET_STATUS = "outletStatus"
DEFAULT_PRODUCT_ID = 2012


'''
run
@param user: User
turns all the user's default devices off
'''
def run(user):
    # turn off all devices with the default product id
    devices = user.getDevicesByProductId(DEFAULT_PRODUCT_ID)
    for device in devices:
        turnOffOutletStatus(device)


'''
turnOffOutletStatus
@param device: Device
turn device's outletStatus off
'''
def turnOffOutletStatus(device):
    device.sendCommand(OUTLET_STATUS, None, OFF)