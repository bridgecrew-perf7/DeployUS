import requests
import pathlib
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
# def test_insert_script_normal(db):
#     name = 'myscript'
#     filename = 'docker-compose.yml'
#     with open(SAMPLE_PROJECTS_FOLDER.joinpath('helloworld').joinpath(filename)) as file:
#         filecontents = file.read()

#     response = DeployUS.insert_script(name, filename, filecontents)
#     dbscripts = DeployUS.get_scripts().json()

#     assert response.status_code == 200
#     assert len(dbscripts) == 1
#     assert dbscripts[0] == (1, name, filecontents)
