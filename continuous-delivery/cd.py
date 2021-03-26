"""
cd.py

Performs the continuous delivery portion of this project. Sends a GET requests to the WatchUS.
"""
import pathlib
import json
import urllib.request

WATCHUS_PORT = 5001
DOT_MACHINES_PATH = pathlib.Path(__file__).absolute().parent.joinpath(".machines")


# 1. Load the list of workers
with open(DOT_MACHINES_PATH,'r') as file:
    machines = json.load(file)['machines']

# 2. Send a GET requests to all watch us to the /up endpoints
for machine in machines:
    
    # This is where the WatchUS is waiting for a signal
    endpoint = f"http://{machine}:{WATCHUS_PORT}/up"

    # Perform continuous delivery
    urllib.request.urlopen(endpoint)


