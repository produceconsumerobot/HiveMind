import requests
import sys
import json

URL = sys.argv[1]
jsonString = sys.argv[2]
print(jsonString)
payload = json.loads(jsonString)
r = requests.post(url = URL, json = payload)
print(r.text)