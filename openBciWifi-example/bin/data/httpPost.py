import requests
import sys
import json

url = sys.argv[1]
jsonString = sys.argv[2]
print(jsonString)
payload = json.loads(jsonString)
r = requests.post(postAddress, json=payload)
print(r.text)