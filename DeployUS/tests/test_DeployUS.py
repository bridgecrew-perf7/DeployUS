import requests
import subprocess
import pathlib
import hashlib
from conftest import DeployUSInterface

SAMPLE_PROJECTS_FOLDER = pathlib.Path(__file__).parent.absolute().joinpath('../../sample-projects/')

DeployUS = DeployUSInterface()

# =====================================================
#  Testing landing pages
# =====================================================
def test_index_page():
    assert DeployUS.index_page().status_code == 200

def test_workers_page():
    assert DeployUS.workers_page().status_code == 200

def test_launch_page():
    assert DeployUS.launch_page().status_code == 200

# =====================================================
#  Testing getting database tables
# =====================================================
def test_get_scripts():
    response = DeployUS.get_scripts()
    assert response.status_code == 200
    assert response.json().__class__ == list 

def test_get_workers():
    response = DeployUS.get_workers()
    assert response.status_code == 200
    assert response.json().__class__ == list 

def test_get_jobs():
    response = DeployUS.get_jobs()
    assert response.status_code == 200
    assert response.json().__class__ == list 


# =====================================================
#  Testing insertions/deletions of scripts
# =====================================================
def test_insert_script_normal(db):
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
    hash = hashlib.sha256(filecontents).hexdigest()

    response = DeployUS.insert_script(name, filename, filecontents)
    dbscripts = DeployUS.get_scripts().json()

    # Testing reponse. Not testing datetime uploaded because it is server specific.
    assert response.status_code == 200
    assert len(dbscripts) == 1
    assert dbscripts[0][0] == 1 #id
    assert dbscripts[0][1] == name #name
    assert dbscripts[0][3] == hash #filehash

def test_insert_script_not_yaml(db):
    name = 'myscript'
    filename = 'scriptname.notyaml'
    filecontents = b"""
        blah blah blah
    """
    hash = hashlib.sha256(filecontents).hexdigest()

    response = DeployUS.insert_script(name, filename, filecontents)
    dbscripts = DeployUS.get_scripts().json()

    # Testing reponse. No file should be added to the database
    assert response.status_code == 200
    assert len(dbscripts) == 0

def test_delete_script_normal(db):
    # Insert script into database
    test_insert_script_normal(db)
    dbscripts = DeployUS.get_scripts().json()
    assert len(dbscripts) == 1

    # Send command to delete script with id 1
    response = DeployUS.delete_script(1)

    # Assertions that script table is empty
    dbscripts = DeployUS.get_scripts().json()
    assert response.status_code == 200
    assert len(dbscripts) == 0


# =====================================================
#  Testing insertions/deletions of workers
# =====================================================
def test_insert_worker_normal(db):
    name = 'worker1'
    location = '123.456.7.8'

    response = DeployUS.insert_worker(name, location)
    dbworkers = DeployUS.get_workers().json()

    # Testing reponse. Not testing datetime uploaded because it is server specific.
    assert response.status_code == 200
    assert len(dbworkers) == 2

    # Part 0: specifics
    assert dbworkers[0][0] == 1 #id
    assert dbworkers[0][1] == 'localhost' #name
    assert dbworkers[0][2] ==  '127.0.0.1' #location

    assert dbworkers[1][0] == 2 #id
    assert dbworkers[1][1] == name #name
    assert dbworkers[1][2] == location #location

def test_delete_worker_normal(db):
    # Inserting worker
    test_insert_worker_normal(db)

    response = DeployUS.delete_worker(worker_id=2)
    dbworkers = DeployUS.get_workers().json()

    # Testing reponse. Not testing datetime uploaded because it is server specific.
    assert response.status_code == 200
    assert len(dbworkers) == 1

def test_delete_worker_non_existant(db):
    # Inserting worker
    test_insert_worker_normal(db)

    response = DeployUS.delete_worker(worker_id=10) #This id does not exists in db
    dbworkers = DeployUS.get_workers().json()

    # Testing reponse. Not testing datetime uploaded because it is server specific.
    assert response.status_code == 200
    assert len(dbworkers) == 2


# =====================================================
#  Testing launch/stopings of jobs
# =====================================================
def test_launch_and_stop_job_normal(db):
    # Inserting the hello-world script
    test_insert_script_normal(db)
    script_id = 1
    worker_id = 1 #localhost

    response = DeployUS.launch_job(script_id, worker_id)
    dbjobs = DeployUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 1

    # Testing entry in database
    assert dbjobs[0][0] == 1 #job id
    assert dbjobs[0][1] == script_id # script id
    assert dbjobs[0][2] == worker_id # worker id

    # Testing if the hello-world application is functioning
    ps = subprocess.Popen(('docker', 'ps'), stdout=subprocess.PIPE)
    result = subprocess.check_output(('grep', 'myscript'), stdin=ps.stdout)
    ps.wait()
    assert len(result) > 0 # The script is started!

    # Stopping the job
    response = DeployUS.stop_job(job_id=1)
    dbjobs = DeployUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 0