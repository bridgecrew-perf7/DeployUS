import requests
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
#  Testing insertions of scripts
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

