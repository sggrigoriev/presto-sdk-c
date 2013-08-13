'''
Created on Aug 7, 2013
@author: Arun Varma
'''
import datetime, pytz, dateutil.parser as dateutil, time


DEFAULT_TIME = 5


def run(user):
    devices = user.getDevicesByProductId(2012)
    for device in devices:
        timerOff(device, DEFAULT_TIME)


def timerOff(device, minutes):
    lastUpdate = isoToDate(device.getParameter("outletStatus")["lastUpdateTime"])
    delta = datetime.timedelta(minutes / 1440)
    turnOffTime = lastUpdate + delta

    tz = pytz.timezone("MST")
    now = tz.localize(datetime.datetime.now())
    timeTillOff = (turnOffTime - now).total_seconds()
    print(timeTillOff)

    if timeTillOff >= 0:
        time.sleep(timeTillOff)
    turnOffOutletStatus(device)


def turnOffOutletStatus(device):
    device.sendCommand("outletStatus", None, "OFF")


def turnOnOutletStatus(device):
    device.sendCommand("outletStatus", None, "ON")


def switchOutletStatus(device):
    if device.getParameter("outletStatus")["value"] == "ON":
        device.sendCommand("outletStatus", None, "OFF")
    else:
        device.sendCommand("outletStatus", None, "ON")
    return device.populateParams(["outletStatus"])


def isoToDate(iso):
    return dateutil.parse(iso)