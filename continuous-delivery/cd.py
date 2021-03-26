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
    machines = json.load(file)

# 2. Deliver DeployUS
DeployUS_location = pathlib.Path(__file__).absolute().parent.joinpath("../DeployUS/docker-compose.yml")
DeployUS_url = f"http://{machines['DeployUS']}:{WATCHUS_PORT}/up"
with open(DeployUS_location) as compose_file:
    DeployUS_file = compose_file.read()
DeployUS_dict = {"file":DeployUS_file}
DeployUS_json = json.dumps(DeployUS_dict).encode('utf-8')
resp = urllib.request.urlopen(DeployUS_url, data=DeployUS_json)


