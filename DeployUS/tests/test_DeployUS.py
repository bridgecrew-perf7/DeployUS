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
