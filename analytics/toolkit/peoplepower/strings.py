'''
strings
Created on Jul 23, 2013
@author: Arun Varma
'''


'''
URLs
'''
# root
URL_ROOT = "alpha2.peoplepowerco.com"
# extention
URL_EXT = "/cloud/json/"
# user
USER = URL_EXT + "user/"
USER_LOGIN = URL_EXT + "login?username="
USER_LOGOUT = URL_EXT + "logout/"
EXPIRY = "&expiry="
# device
DEVICES = URL_EXT + "devices/"
DEVICE_PRODUCT_ID = "?productId="
# location
LOCATIONS = URL_EXT + "locations/"
LOCATION_DEVICES = "/devices/"


'''
HTTP Requests
'''
GET = "GET"
POST = "POST"
PUT = "PUT"
DELETE = "DELETE"


'''
Body/Header
'''
API_KEY = "PRESENCE_API_KEY"
USERNAME = "username"
PASSWORD = "PASSWORD"
CONTENT_TYPE = "Content-Type"
JSON_APPLICATION = "application/json"


'''
Misc
'''
DECODER = "utf-8"