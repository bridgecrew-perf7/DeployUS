"""
cd.py

Performs the continuous delivery portion of this project. Sends a GET requests to the WatchUS.
"""
import pathlib
import json
import urllib.request
import sys

WATCHUS_PORT = 5001
DOT_MACHINES_PATH = pathlib.Path(__file__).absolute().parent.joinpath(".machines")
DEPLOYUS = 'DeployUS'
WORKUS = 'WorkUS'

def deliver(addr, module):
    """
    Deliver a module to the address specified. The location must have an active WatchUS.
    addr: string. Ex: "34.122.129.63"
    module: string. Ex: 'DeployUS'
    """
    # Initialize key variables
    dc_disk_location = pathlib.Path(__file__).absolute().parent.joinpath(f"../{module}/docker-compose.yml")
    watchus_url = f"http://{addr}:{WATCHUS_PORT}/up"

    # Load the contents of the docker-compose file
    with open(dc_disk_location) as compose_file:
        dc_file_contents = compose_file.read()

    # Creating json object
    dc_dict = {"file":dc_file_contents}
    dc_json = json.dumps(dc_dict)

    # Sending module to the distant machine
    print(f"Delivering {module} at {addr}.")
    resp = urllib.request.urlopen(watchus_url, data=dc_json.encode('ascii'))

    # Return if an error occured
    return resp.status != 200

# 1. Load the list of workers
with open(DOT_MACHINES_PATH,'r') as file:
    machines = json.load(file)

# 2. Initialize exit code
exit_code_total = False

# 2. Deliver DeployUS
exit_code = deliver(machines[DEPLOYUS], DEPLOYUS)
exit_code_total |= exit_code
if exit_code:
    print(f'Failed to deliver DeployUS at {machines[DEPLOYUS]}.')

# 3. Deliver all WorkUS
for machine_addr in machines[WORKUS]:
    exit_code = deliver(machine_addr, WORKUS)
    exit_code_total |= exit_code
    if exit_code:
        print(f'Failed to deliver WorkUS at {machine_addr}.')

# 4. Set script exit code for CI
sys.exit(1 if exit_code_total else 0)


