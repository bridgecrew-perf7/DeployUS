"""
DeployUS/tests/test_DeployUS.py

Uses pytest to perform behavioral test of DeployUS's REST API.

To run:
    - python3 run_tests.py
"""
import subprocess
import pathlib
import hashlib
from conftest import DeployUSInterface

SAMPLE_PROJECTS_FOLDER = pathlib.Path(__file__).parent.absolute().joinpath('../../sample-projects/')
DEPLOYUS = DeployUSInterface()

# =====================================================
#  Testing landing pages
# =====================================================
def test_index_page():
    """
    Sending GET request to the / page to see if a 200 code is obtained.
    """
    assert DEPLOYUS.index_page().status_code == 200

def test_workers_page():
    """
    Sending GET request to the /workers page to see if a 200 code is obtained.
    """
    assert DEPLOYUS.workers_page().status_code == 200

def test_launch_page():
    """
    Sending GET request to the /launch page to see if a 200 code is obtained.
    """
    assert DEPLOYUS.launch_page().status_code == 200

# =====================================================
#  Testing getting database tables
# =====================================================
def test_get_scripts():
    """
    Sending GET request to /get_scripts to see if a list of scripts is stored in json field.
    """
    response = DEPLOYUS.get_scripts()
    assert response.status_code == 200
    assert response.json().__class__ == list

def test_get_workers():
    """
    Sending GET request to /get_workers to see if a list of workers is stored in json field.
    """
    response = DEPLOYUS.get_workers()
    assert response.status_code == 200
    assert response.json().__class__ == list

def test_get_jobs():
    """
    Sending GET request to /get_jobs to see if a list of jobs is stored in json field.
    """
    response = DEPLOYUS.get_jobs()
    assert response.status_code == 200
    assert response.json().__class__ == list


# =====================================================
#  Testing insertions/deletions of scripts
# =====================================================
def test_insert_script_normal():
    """
    Creating a hello world example docker-compose project
    and uploading it to DeployUS.

    Testing the state of the mysql database.
    """
    name = 'myscript'
    filename = 'docker-compose.yml'
    filecontents = b"""
        version: "3.3"
        services:
          app:
            image: shawnvosburg/helloworld:latest
            ports:
              - "8000:80"
    """

    # Testing hash of filecontents. This step ensures that the file transmitted wasn't corrupted.
    hash1 = hashlib.sha256(filecontents).hexdigest()

    # Sends POST request with file to DeployUS. Queries database for script table.
    response = DEPLOYUS.insert_script(name, filename, filecontents)
    dbscripts = DEPLOYUS.get_scripts().json()

    # Testing reponse. Not testing datetime uploaded because it is server specific.
    assert response.status_code == 200
    assert len(dbscripts) == 1
    assert dbscripts[0][0] == 1 #id
    assert dbscripts[0][1] == name #name
    assert dbscripts[0][3] == hash1 #filehash

def test_insert_script_normal_multiple():
    """
    Creates two helloworld projects that open different ports (8000 and 8001 respectively).
    Their names are also different (myscript1 and myscript2 respectively).

    The file name is the standard docker-compose.yml.

    Uploads the scripts to DeployUS and queries database for sucessful upload.
    """
    name1 = 'myscript1'
    name2 = 'myscript2'
    filename = 'docker-compose.yml'
    filecontents1 = b"""
        version: "3.3"
        services:
          app:
            image: shawnvosburg/helloworld:latest
            ports:
              - "8000:80"
    """
    filecontents2 = b"""
        version: "3.3"
        services:
          app:
            image: shawnvosburg/helloworld:latest
            ports:
              - "8001:80"
    """

    # Calcualte hash to compare with database to ensure
    # that the file has not been corrupted during upload.
    hash1 = hashlib.sha256(filecontents1).hexdigest()
    hash2 = hashlib.sha256(filecontents2).hexdigest()

    # Uploading scripts to DeployUS and querying state of database.
    DEPLOYUS.insert_script(name1, filename, filecontents1)
    response = DEPLOYUS.insert_script(name2, filename, filecontents2)
    dbscripts = DEPLOYUS.get_scripts().json()

    # Testing reponse. Not testing datetime uploaded because it is server specific.
    assert response.status_code == 200  # Successful reponse
    assert len(dbscripts) == 2          # The two scripts must be added
    assert dbscripts[0][0] == 1         # id myscript1
    assert dbscripts[0][1] == name1     # myscript1
    assert dbscripts[0][3] == hash1     # filehash of myscript1's script
    assert dbscripts[1][0] == 2         # id myscript2
    assert dbscripts[1][1] == name2     # myscript2
    assert dbscripts[1][3] == hash2     # filehash of myscript2's script


def test_insert_script_not_yaml():
    """
    Verifies that only .yml files can be uploaded.

    DeployUS should still return status_code 200.
    It should just not insert the script into the database.

    No warning should be given by DeployUS about the wrong file extension.
    Other than a 422 status code.
    """
    name = 'myscript'
    filename = 'scriptname.notyaml'
    filecontents = b"""
        version: "3.3"
        services:
          app:
            image: shawnvosburg/helloworld:latest
            ports:
              - "8000:80"
    """

    # Attempting to insert script and querying database for resulting state.
    response = DEPLOYUS.insert_script(name, filename, filecontents)
    dbscripts = DEPLOYUS.get_scripts().json()

    # Testing reponse. No file should be added to the database
    assert response.status_code == 422
    assert len(dbscripts) == 0

def test_insert_script_unique_name():
    """
    Verifying that a user can not give the same name
    to two different docker-compose scripts.
    """
    name = 'myscript'
    filename1 = 'docker-compose1.yml'
    filename2 = 'docker-compose2.yml'
    filecontents1 = b"""
        version: "3.3"
        services:
          app:
            image: shawnvosburg/helloworld:latest
            ports:
              - "8000:80"
    """
    filecontents2 = b"""
        version: "3.3"
        services:
          app:
            image: shawnvosburg/helloworld:latest
            ports:
              - "8001:80"
    """

    # Adding script
    response = DEPLOYUS.insert_script(name, filename1, filecontents1)
    dbscripts = DEPLOYUS.get_scripts().json()

    # Testing reponse. Should be a success
    assert response.status_code == 200
    assert len(dbscripts) == 1

    # Adding script again
    response = DEPLOYUS.insert_script(name, filename2, filecontents2)
    dbscripts = DEPLOYUS.get_scripts().json()

    # Testing reponse. The script should not be added again as the name is the same
    assert response.status_code == 200
    assert len(dbscripts) == 1

def test_delete_script_normal():
    """
    Verifying that deleting a script works in DeployUS.

    An inserted script that has been deleted should not be in
    the database after deleted.
    """

    # Insert script into database
    test_insert_script_normal()
    dbscripts = DEPLOYUS.get_scripts().json()
    assert len(dbscripts) == 1

    # Send command to delete script with id 1 (AUTO_INCREMENT value of mysql)
    response = DEPLOYUS.delete_script(1)

    # Assertions that script table is empty
    dbscripts = DEPLOYUS.get_scripts().json()
    assert response.status_code == 200
    assert len(dbscripts) == 0


# =====================================================
#  Testing insertions/deletions of workers
# =====================================================
def test_insert_worker_normal():
    """
    Verifying that a worker and its address (known as location in DeployUS) can be added.
    """
    name = 'worker1'
    location = '123.456.7.8'

    # Insertion through DeployUS and querying resultant state of database
    response = DEPLOYUS.insert_worker(name, location)
    dbworkers = DEPLOYUS.get_workers().json()

    # Testing reponse. Worker should be added
    assert response.status_code == 200
    assert len(dbworkers) == 2

    # Part 0: specifics
    assert dbworkers[0][0] == 1 #id
    assert dbworkers[0][1] == 'localhost' #name
    assert dbworkers[0][2] ==  '127.0.0.1' #location

    # Verifying resultant state of database
    assert dbworkers[1][0] == 2 #id
    assert dbworkers[1][1] == name #name
    assert dbworkers[1][2] == location #location

def test_insert_worker_unique_name():
    """
    DeployUS should refuse the second insertion of
    workers being named the same.

    A 422 status code should be given as failure of insertion.
    """
    name = 'worker1'
    location1 = '123.456.7.8'
    location2 = '987.654.3.2'

    # Adding two workers with the same name
    DEPLOYUS.insert_worker(name, location1)
    response = DEPLOYUS.insert_worker(name, location2)
    dbworkers = DEPLOYUS.get_workers().json()

    # Testing reponse. Worker should be added.
    assert response.status_code == 422
    assert len(dbworkers) == 2

    assert dbworkers[1][0] == 2 #id
    assert dbworkers[1][1] == name #name
    assert dbworkers[1][2] == location1 #

def test_insert_worker_unique_location():
    """
    DeployUS should refuse the insertion of the second worker
    as the location is the same as the first

    A 422 status code should be given as failure of insertion.
    """
    name1 = 'worker1'
    name2 = 'worker2'
    location = '123.456.7.8'

    # Adding two workers with the same name
    DEPLOYUS.insert_worker(name1, location)
    response = DEPLOYUS.insert_worker(name2, location)
    dbworkers = DEPLOYUS.get_workers().json()

    # Testing reponse. Worker should be added.
    assert response.status_code == 422
    assert len(dbworkers) == 2
    assert dbworkers[1][0] == 2 #id
    assert dbworkers[1][1] == name1 #name
    assert dbworkers[1][2] == location #location

def test_delete_worker_normal():
    """
    Inserting a new worker and testing that it can be deleted sucessfully.
    """
    # Inserting worker
    test_insert_worker_normal()

    # Deletion of worker and querying result with database
    response = DEPLOYUS.delete_worker(worker_id=2)
    dbworkers = DEPLOYUS.get_workers().json()

    # Testing reponse. Not testing datetime uploaded because it is server specific.
    assert response.status_code == 200
    assert len(dbworkers) == 1

def test_delete_worker_non_existant():
    """
    The deletion of worker that does not exists should return a
    200 status code as the final state of the database is that the worker is not present.
    """
    # Inserting worker
    test_insert_worker_normal()

    response = DEPLOYUS.delete_worker(worker_id=10) #This id does not exists in db
    dbworkers = DEPLOYUS.get_workers().json()

    # Testing reponse. Not testing datetime uploaded because it is server specific.
    assert response.status_code == 200
    assert len(dbworkers) == 2


# =====================================================
#  Testing launch/stopings of jobs
# =====================================================
def test_launch_and_stop_job_normal():
    """
    Launching a job with a basic helloworld project.
    Verifying with docker that the docker-compose file is up.

    Stopping project afterwards as to not affect the other tests.
    """

    # Inserting the hello-world script
    test_insert_script_normal()
    script_id = 1 # Following MySQL AUTO_INCREMENT convention
    worker_id = 1 # localhost

    response = DEPLOYUS.launch_job(script_id, worker_id)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 1

    # Testing entry in database
    assert dbjobs[0][0] == 1 #job id
    assert dbjobs[0][1] == script_id # script id
    assert dbjobs[0][2] == worker_id # worker id

    # Testing if the hello-world application is functioning
    ps_ = subprocess.Popen(('docker', 'ps'), stdout=subprocess.PIPE)
    result = subprocess.check_output(('grep', 'myscript'), stdin=ps_.stdout)
    ps_.wait()
    assert len(result) > 0 # The script is started!

    # Stopping the job
    response = DEPLOYUS.stop_job(job_id=1)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 0

def test_launch_and_stop_job_normal_multiple():
    """
    Launching a job with two basic helloworld projects.
    Verifying with docker that the docker-compose files is up.

    Stopping projects afterwards as to not affect the other tests.
    """
    # Inserting the hello-world script multiple times
    test_insert_script_normal_multiple()
    DEPLOYUS.launch_job(script_id=1, worker_id=1)
    response = DEPLOYUS.launch_job(script_id=2, worker_id=1)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 2

    # Testing entries in database
    assert dbjobs[0][0] == 1 #job id
    assert dbjobs[0][1] == 1 # script id
    assert dbjobs[0][2] == 1 # worker id
    assert dbjobs[1][0] == 2 #job id
    assert dbjobs[1][1] == 2 # script id
    assert dbjobs[1][2] == 1 # worker id

    # Testing if myscript1 is functioning
    ps_ = subprocess.Popen(('docker', 'ps'), stdout=subprocess.PIPE)
    result = subprocess.check_output(('grep', 'myscript1'), stdin=ps_.stdout)
    ps_.wait()
    assert len(result) > 0 # The script is started!

    # Testing if myscript2 is functioning
    ps_ = subprocess.Popen(('docker', 'ps'), stdout=subprocess.PIPE)
    result = subprocess.check_output(('grep', 'myscript1'), stdin=ps_.stdout)
    ps_.wait()
    assert len(result) > 0 # The script is started!

    # Stopping the jobs
    DEPLOYUS.stop_job(job_id=1)
    response = DEPLOYUS.stop_job(job_id=2)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 0

def test_launch_job_bad_script_id():
    """
    A docker-compose file should not launch if the script does not
    exists in DeployUS
    """

    # Inserting the hello-world script
    test_insert_script_normal()
    script_id = 10 # This script doesn't exists. Therefore, the job should not launch
    worker_id = 1 # localhost

    response = DEPLOYUS.launch_job(script_id, worker_id)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 422
    assert len(dbjobs) == 0

def test_launch_job_bad_worker_id():
    """
    A docker-compose file should not launch if the worker does not
    exists in DeployUS
    """

    # Inserting the hello-world script
    test_insert_script_normal()
    script_id = 1
    worker_id = 10 # This worker doesn't exists. Therefore, the job should not launch

    response = DEPLOYUS.launch_job(script_id, worker_id)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 422
    assert len(dbjobs) == 0

def test_launch_job_failed_execution():
    """
    If launching a job has a runtime error (like image of docker hub does not exists),
    DeployUS should send back a 422 status code.
    """

    # Inserting the hello-world script
    name = 'myscript'
    filename = 'docker-compose.yml'
    # The image in the docker-compose file does not exists on docker hub
    filecontents = b"""
        version: "3.3"
        services:
          app:
            image: shawnvosburg/image-dne
            ports:
              - "8000:80"
    """

    # Sends POST request with file to DeployUS.
    DEPLOYUS.insert_script(name, filename, filecontents)

    # Attempt to launch job. Query database for launched jobs.
    script_id = 1
    worker_id = 1 # localhost
    response = DEPLOYUS.launch_job(script_id, worker_id)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 422
    assert len(dbjobs) == 0
