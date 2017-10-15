import requests
import sys

URL = sys.argv[1]
r = requests.get(url = URL)
print(r.text)