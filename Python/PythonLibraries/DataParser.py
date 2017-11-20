def parseData(data):
    parsedData = {}

    for command in data.split(','):
        parsedData[command.split(':')[0]] = command.split(':')[1]

    return parsedData

def createDataPacket(data):
    packet = ""
    for key in data:
        packet = packet + key + ":" + str(data[key]) + "\n"
    return packet
