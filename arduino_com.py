import serial

cmdcharacter = 255

try:
    ser = serial.Serial('/dev/ttyACM0', 9600)
except Exception:
    ser = serial.Serial('/dev/ttyACM1', 9600)

def sendcmd(values):
    values = values[:4]
    while len(values) < 4:
        values.append(128)
    for value in values:
        if value > 255:
            value = 255
        if value < 0:
            value = 0
    ser.write(bytearray([cmdcharacter] + values))


if __name__ == "__main__":
    while True:
        lst = []
        print("Sending new command!")
        for i in range(4):
            lst.append(int(input("Channel " + str(i) + " >")))
        print("Sending command: ")
        print(lst)
        sendcmd(lst)
