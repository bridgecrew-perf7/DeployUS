"""
DeployUS/tests/test_persistance_data.py

Uses pytest to perform behavioral test of DeployUS's REST API.

To run:
    - python3 run_tests.py
"""
import subprocess
import pytest
from conftest import DeployUSInterface
from test_scripts import test_insert_script_normal

DEPLOYUS = DeployUSInterface()

# =====================================================
#  Testing persistance data
# =====================================================
@pytest.mark.usefixtures("_db")
def test_persistance_data(_db):
    """
    This test inserts a new script into the database through DeployUS,
    shuts down DeployUS via         docker-compose kill,
    and boots DeployUS back up via  docker-compose up.

    If the data is still present, then the application is said
    to have persistance of data. This is what we want.

    We are not using the global fixture, but one where the db object is
    passed as a parameter.
    """
    # Insertion of a hello-world script.
    test_insert_script_normal()

    # Shut down DeployUS via docker-compose
    cmd = [
        "docker-compose",
        "-f",
        "/DeployUS/docker-compose.tests.yml",
        "kill",
        "app",
        "db",
    ]
    assert subprocess.call(cmd) == 0

    # Boot DeployUS back up.
    cmd = [
        "docker-compose",
        "-f",
        "/DeployUS/docker-compose.tests.yml",
        "up",
        "-d",
        "--no-build",
        "db",
        "app",
    ]
    assert subprocess.call(cmd) == 0
    subprocess.call(["/wait-for-it.sh", "app:5000", "--strict", "--timeout=30"])

    # Re-initialize connection to database
    _db.__init__()

    # Assert the data is still in the database
    dbscripts = DEPLOYUS.get_scripts().json()
    assert len(dbscripts) == 1
    assert dbscripts[0][0] == 1  # id
    assert dbscripts[0][1] == "myscript"  # name
