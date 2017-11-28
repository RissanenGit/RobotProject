def parseData(data):
    parsedData = {}
    try:
        for command in data.split(','):
            parsedData[command.split(':')[0]] = command.split(':')[1]
        return parsedData
    except Exception:
        return False

def createServerPacket(data):
    packet = ""
    for key in data:
        if(key == "EventType"):
            packet = packet + key + ":" + getEventType(key) + "\n"
        else:
            packet = packet + key + ":" + str(data[key]) + "\n"
    return packet

def getEventType(data):
    if data["EventType"] == 1:
        return "Forward"
    elif data["EventType"] == 2:
        return "BackWards"
    elif data["EventType"] == 3:
        return "Left"
    elif data["EventType"] == 4:
        return "Right"
    else:
        return "UnkownEvent"
def createDatabasePacket(data):

    message = b'------Boundary\r\nContent-Disposition: form-data; name="idrobot"\r\n\r\n4\r\n'
    message = message + b'------Boundary\r\nContent-Disposition: form-data; name="salasana"\r\n\r\n123\r\n'
    message = message + b'------Boundary\r\nContent-Disposition: form-data; name="event_type"\r\n\r\n' + data["EventType"]+ '\r\n'
    message = message + b'------Boundary\r\nContent-Disposition: form-data; name="event_data"\r\n\r\n' + data["EventData"]+'\r\n'
    message = message + b'------Boundary\r\nContent-Disposition: form-data; name="akku"\r\n\r\n' + str(data["BatteryLevel"]) + '\r\n'
    message = message + b'------Boundary--\r\n'

    headers = b"POST /~c5vaha00/robottiprojekti/index.php/Input/tarkista_syotto HTTP/1.1\r\nHost: 193.167.100.74:80\r\nContent-Type: multipart/form-data; boundary=----Boundary\r\nContent-Length: " + str(len(message)) + "\r\n\r\n"

    return (headers + message)
