'''
Created on Aug 14, 2013
@author: Arun Varma
'''
import time
import sys


ON = "ON"
OFF = "OFF"
OUTLET_STATUS = "outletStatus"
DOT = "."
DASH = "-"
PAUSE = " "
SHORT = 0.5
LONG = 2
DEFAULT_PRODUCT_ID = 2012
DEFAULT_MESSAGE = "SOS"
morseCode = {
        'A': '.-',              'a': '.-',
        'B': '-...',            'b': '-...',
        'C': '-.-.',            'c': '-.-.',
        'D': '-..',             'd': '-..',
        'E': '.',               'e': '.',
        'F': '..-.',            'f': '..-.',
        'G': '--.',             'g': '--.',
        'H': '....',            'h': '....',
        'I': '..',              'i': '..',
        'J': '.---',            'j': '.---',
        'K': '-.-',             'k': '-.-',
        'L': '.-..',            'l': '.-..',
        'M': '--',              'm': '--',
        'N': '-.',              'n': '-.',
        'O': '---',             'o': '---',
        'P': '.--.',            'p': '.--.',
        'Q': '--.-',            'q': '--.-',
        'R': '.-.',             'r': '.-.',
        'S': '...',             's': '...',
        'T': '-',               't': '-',
        'U': '..-',             'u': '..-',
        'V': '...-',            'v': '...-',
        'W': '.--',             'w': '.--',
        'X': '-..-',            'x': '-..-',
        'Y': '-.--',            'y': '-.--',
        'Z': '--..',            'z': '--..',
        '0': '-----',           ',': '--..--',
        '1': '.----',           '.': '.-.-.-',
        '2': '..---',           '?': '..--..',
        '3': '...--',           ';': '-.-.-.',
        '4': '....-',           ':': '---...',
        '5': '.....',           "'": '.----.',
        '6': '-....',           '-': '-....-',
        '7': '--...',           '/': '-..-.',
        '8': '---..',           '(': '-.--.-',
        '9': '----.',           ')': '-.--.-',
        ' ': ' ',               '_': '..--.-',
}


'''
run
@param user: User
turns the user's default devices off automatically after the default time
'''
def run(user):
    # timerOff all devices with the default product id
    devices = user.getDevicesByProductId(DEFAULT_PRODUCT_ID)
    raw_input = input("Enter a message: ")
    if raw_input == "":
        toTranslate = DEFAULT_MESSAGE
    else:
        toTranslate = raw_input

    for device in devices:
        morseFunction(device, toTranslate)


'''
morseFunction
@param device: Device
@param message: String
turns the given device on and off according to morse code, depending on the given message
'''
def morseFunction(device, message):
    # turn off device initially
    turnOffOutletStatus(device)

    # convert message to morse code
    morse = toMorse(message)
    sys.stdout.write(message + ": ")
    sys.stdout.flush()
    # for each character, 
    for character in morse:
        morseSwitch(device, character)
        sys.stdout.write(character)
        sys.stdout.flush()


'''
morseSwitch
@param character: char
turns on/off the outletStatus depending on character
'''
def morseSwitch(device, character):
    if character == DOT:
        turnOnOutletStatus(device)
        time.sleep(SHORT)
        turnOffOutletStatus(device)
    elif character == DASH:
        turnOnOutletStatus(device)
        time.sleep(LONG)
        turnOffOutletStatus(device)
    elif character == PAUSE:
        time.sleep(SHORT)


'''
turnOffOutletStatus
@param device: Device
turn device's outletStatus off
'''
def turnOffOutletStatus(device):
    device.sendCommand(OUTLET_STATUS, None, OFF)


'''
turnOnOutletStatus
turn device's outletStatus off
'''
def turnOnOutletStatus(device):
    device.sendCommand(OUTLET_STATUS, None, ON)


'''
toMorse
@param message: String
@return the given message in morse code
'''
def toMorse(message):
    morse = ""
    for character in message:
        try:
            morse += morseCode[character]
        except:
            print("Character " + character + " is not supported by morse code")
    return morse