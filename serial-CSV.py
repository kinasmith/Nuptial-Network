#likely the best bet is to dump data into an expanding CSV
#Then run this conversion on it at intervals, such as every 5 min to get a JSON out of it
#Then run an R Script to convert to a Dataframe of some sort.
#Todo:
#Write script to open serial port and save serial stream into a .csv format
#write second script to convert to a JSON file which will run at intervals
#give JSON data to Molly to work with.


import serial
import time
import datetime
import json

ser = serial.Serial('/dev/ttyACM0', 115200)
while 1:
    now = datetime.datetime.now().replace(microsecond=0).isoformat()
    outCSV = open("data/data.csv", 'a')
    inString = ser.readline()
    outCSV.write(now)
    outCSV.write(",")
    outCSV.write(inString)
    outCSV.flush()
