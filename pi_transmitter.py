import RPi.GPIO as GPIO
import sys
import time

code_dict = { 'A':'.-', 'B':'-...',
                    'C':'-.-.', 'D':'-..', 'E':'.',
                    'F':'..-.', 'G':'--.', 'H':'....',
                    'I':'..', 'J':'.---', 'K':'-.-',
                    'L':'.-..', 'M':'--', 'N':'-.',
                    'O':'---', 'P':'.--.', 'Q':'--.-',
                    'R':'.-.', 'S':'...', 'T':'-',
                    'U':'..-', 'V':'...-', 'W':'.--',
                    'X':'-..-', 'Y':'-.--', 'Z':'--..',
                    '1':'.----', '2':'..---', '3':'...--',
                    '4':'....-', '5':'.....', '6':'-....',
                    '7':'--...', '8':'---..', '9':'----.',
                    '0':'-----', ', ':'--..--', '.':'.-.-.-',
                    '?':'..--..', '/':'-..-.', '-':'-....-',
                    '(':'-.--.', ')':'-.--.-'}

DOT_TIME_LEN = 0.0035
GPIO_PIN_NUM = 36

def send_dot(channel):
    GPIO.output(channel, GPIO.HIGH)
    time.sleep(DOT_TIME_LEN)
    GPIO.output(channel, GPIO.LOW)
    time.sleep(DOT_TIME_LEN)

def send_dash(channel):
    GPIO.output(channel, GPIO.HIGH)
    time.sleep(DOT_TIME_LEN * 3)
    GPIO.output(channel, GPIO.LOW)
    time.sleep(DOT_TIME_LEN)


def to_morse(message):
    s = ""
    for char in message.upper():
        if char == ' ':
            s += '/'
        else: 
            s += code_dict[char]
        s += ' '
    return s

def transmit(channel, message):
    for char in message:
        if char == '.':
            send_dot(channel)
        elif char == '-':
            send_dash(channel)
        elif char == ' ':
            time.sleep(DOT_TIME_LEN * 2)
        elif char == '/':
            time.sleep(DOT_TIME_LEN * 2)

def main():
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(GPIO_PIN_NUM, GPIO.OUT)
    num_lines = sys.argv[1]
    message = sys.argv[2]
    morse = to_morse(message)
    for i in range(int(num_lines)):
        print(morse)
        transmit(GPIO_PIN_NUM, morse)
    GPIO.cleanup()


if __name__ == "__main__":
    main()


