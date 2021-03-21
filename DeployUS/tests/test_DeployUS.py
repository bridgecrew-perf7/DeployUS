import requests
from conftest import DeployUSInterface

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
#  Testing geting database tables
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
