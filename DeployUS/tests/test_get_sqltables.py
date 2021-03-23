"""
DeployUS/tests/test_sqltables.py

Uses pytest to perform behavioral test of DeployUS's REST API.
Testing endpoints which returns the contents of the mysql tables.

To run:
    - python3 run_tests.py
"""
import pytest
from conftest import DeployUSInterface

DEPLOYUS = DeployUSInterface()

# =====================================================
#  Testing getting database tables
# =====================================================
@pytest.mark.usefixtures('_db')
def test_get_scripts():
    """
    Sending GET request to /get_scripts to see if a list of scripts is stored in json field.
    """
    response = DEPLOYUS.get_scripts()
    assert response.status_code == 200
    assert response.json().__class__ == list

@pytest.mark.usefixtures('_db')
def test_get_workers():
    """
    Sending GET request to /get_workers to see if a list of workers is stored in json field.
    """
    response = DEPLOYUS.get_workers()
    assert response.status_code == 200
    assert response.json().__class__ == list

@pytest.mark.usefixtures('_db')
def test_get_jobs():
    """
    Sending GET request to /get_jobs to see if a list of jobs is stored in json field.
    """
    response = DEPLOYUS.get_jobs()
    assert response.status_code == 200
    assert response.json().__class__ == list
