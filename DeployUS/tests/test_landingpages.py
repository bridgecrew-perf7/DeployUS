"""
DeployUS/tests/test_landingpages.py

Uses pytest to perform behavioral test of ClientUS's endpoints.

To run:
    - python3 run_tests.py
"""
import pytest
from conftest import DeployUSInterface

DEPLOYUS = DeployUSInterface()

# =====================================================
#  Testing landing pages
# =====================================================
@pytest.mark.usefixtures('_db')
def test_index_page():
    """
    Sending GET request to the / page to see if a 200 code is obtained.
    """
    assert DEPLOYUS.index_page().status_code == 200

@pytest.mark.usefixtures('_db')
def test_workers_page():
    """
    Sending GET request to the /workers page to see if a 200 code is obtained.
    """
    assert DEPLOYUS.workers_page().status_code == 200

@pytest.mark.usefixtures('_db')
def test_launch_page():
    """
    Sending GET request to the /launch page to see if a 200 code is obtained.
    """
    assert DEPLOYUS.launch_page().status_code == 200
