import json
import csv
# This is a comment
with open('data/data.csv', 'rb') as csvfile:
    data={}
    data['data']=[]
    reader = csv.reader(csvfile, delimiter=",")
    for row in reader:
        del row[-1]
        timestamp = row[0]
        pollingNode = row[1]
        bv = row[2]

        node = {}
        node['timestamp'] = timestamp
        node['pollingNode'] = pollingNode
        node['bv'] = bv
        node['nodes'] = []

        inNode = [
        {'id': row[k-1],'rssi': row[k]} 
        for k in range(4,len(row),2)
        ]

        node['nodes'].append(inNode)
        data['data'].append(node)

outJSON = open("data/data.json", 'w')
json.dump(data, outJSON)
outJSON.flush()
outJSON.close()
