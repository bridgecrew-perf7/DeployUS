"""
DeployUS/tests/test_jobs.py

Uses pytest to perform behavioral test of DeployUS's jobs REST api.

To run:
    - python3 run_tests.py
"""
import subprocess
import urllib.request
import pytest
from conftest import DeployUSInterface
from test_scripts import test_insert_script_normal, test_insert_script_normal_multiple

DEPLOYUS = DeployUSInterface()

# =====================================================
#  Testing launch/stopings of jobs
# =====================================================


@pytest.mark.usefixtures("_db")
def test_launch_and_stop_job_normal():
    """
    Launching a job with a basic helloworld project.
    The job launched opens on port 8000. We will send a GET request and check the html contents.

    Stopping project afterwards as to not affect the other tests.
    """
    # Inserting the workus worker
    resp = DEPLOYUS.insert_worker('w1', 'workus')
    dbworkers = DEPLOYUS.get_workers().json()
    assert resp.status_code == 200
    assert len(dbworkers) == 2 # There is localhost

    # Inserting the hello-world script
    index_contents = test_insert_script_normal()
    script_id = 1  # Following MySQL AUTO_INCREMENT convention
    worker_id = 2  # w1

    response = DEPLOYUS.launch_job(script_id, worker_id)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 1

    # Testing entry in database
    assert dbjobs[0][0] == 1  # job id
    assert dbjobs[0][1] == script_id  # script id
    assert dbjobs[0][2] == worker_id  # worker id

    # Testing if the hello-world application is functioning
    resp = urllib.request.urlopen("http://dummy")
    content =  resp.read().decode('utf-8')
    assert resp.status == 200
    assert index_contents == content.strip() # Must strip because an extra \n is added.

    # Stopping the job
    response = DEPLOYUS.stop_job(job_id=1)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 0


@pytest.mark.usefixtures("_db")
def test_launch_and_stop_job_normal_multiple():
    """
    Launching a job with two basic helloworld projects.
    Verifying with docker that the docker-compose files is up.

    Stopping projects afterwards as to not affect the other tests.
    """
    # Inserting the hello-world script multiple times
    index_contents1, index_contents2 = test_insert_script_normal_multiple()
    DEPLOYUS.launch_job(script_id=1, worker_id=1)
    response = DEPLOYUS.launch_job(script_id=2, worker_id=1)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 2

    # Testing entries in database
    assert dbjobs[0][0] == 1  # job id
    assert dbjobs[0][1] == 1  # script id
    assert dbjobs[0][2] == 1  # worker id
    assert dbjobs[1][0] == 2  # job id
    assert dbjobs[1][1] == 2  # script id
    assert dbjobs[1][2] == 1  # worker id

    # Testing if myscript1 is functioning
    resp = urllib.request.urlopen("http://dummy1")
    content =  resp.read().decode('utf-8')
    assert resp.status == 200
    assert index_contents1 == content.strip() # Must strip because an extra \n is added.

    # Testing if myscript2 is functioning
    resp = urllib.request.urlopen("http://dummy2")
    content =  resp.read().decode('utf-8')
    assert resp.status == 200
    assert index_contents2 == content.strip() # Must strip because an extra \n is added.

    # Stopping the jobs
    DEPLOYUS.stop_job(job_id=1)
    response = DEPLOYUS.stop_job(job_id=2)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 0


@pytest.mark.usefixtures("_db")
def test_launch_job_bad_script_id():
    """
    A docker-compose file should not launch if the script does not
    exists in DeployUS
    """

    # Inserting the hello-world script
    test_insert_script_normal()
    script_id = 10  # This script doesn't exists. Therefore, the job should not launch
    worker_id = 1  # localhost

    response = DEPLOYUS.launch_job(script_id, worker_id)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 422
    assert len(dbjobs) == 0


@pytest.mark.usefixtures("_db")
def test_launch_job_bad_worker_id():
    """
    A docker-compose file should not launch if the worker does not
    exists in DeployUS
    """

    # Inserting the hello-world script
    test_insert_script_normal()
    script_id = 1
    worker_id = 10  # This worker doesn't exists. Therefore, the job should not launch

    response = DEPLOYUS.launch_job(script_id, worker_id)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 422
    assert len(dbjobs) == 0


@pytest.mark.usefixtures("_db")
def test_launch_job_failed_execution():
    """
    If launching a job has a runtime error (like image of docker hub does not exists),
    DeployUS should send back a 422 status code.
    """

    # Inserting the hello-world script
    name = "myscript"
    filename = "docker-compose.yml"
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
    worker_id = 1  # localhost
    response = DEPLOYUS.launch_job(script_id, worker_id)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 422
    assert len(dbjobs) == 0


@pytest.mark.usefixtures("_db")
def test_stop_job_dne():
    """
    Attempt to stop a job that does not exists.
    """

    # Sends POST request to stop job.
    response = DEPLOYUS.stop_job(100)
    assert response.status_code == 422
