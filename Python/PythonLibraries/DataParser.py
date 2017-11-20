def parseData(data):
    parsedData = {}
    try:
        for command in data.split(','):
            parsedData[command.split(':')[0]] = command.split(':')[1]
            return parsedData
    except Exception:
        return False

def createDataPacket(data):
    packet = ""
    for key in data:
        packet = packet + key + ":" + str(data[key]) + "\n"
    return packet
