"""
DeployUS/tests/test_jobs.py

Uses pytest to perform behavioral test of DeployUS's jobs REST api.

To run:
    - python3 run_tests.py
"""
from time import sleep
import pytest
from pytest_httpserver import HTTPServer
from conftest import DeployUSInterface
from test_scripts import test_insert_script_normal, test_insert_script_normal_multiple

DEPLOYUS = DeployUSInterface()
WORKUS_PORT = 5002

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
    # Creating mocked WorkUS instances
    workus1 = HTTPServer(host="workus1",port=WORKUS_PORT)
    workus1.start()

    # Inserting the workus worker
    resp = DEPLOYUS.insert_worker("w1", "workus1")
    dbworkers = DEPLOYUS.get_workers().json()
    assert resp.status_code == 200
    assert len(dbworkers) == 1

    # Inserting the hello-world script
    (script_name, filecontents) = test_insert_script_normal()
    script_id = 1  # Following MySQL AUTO_INCREMENT convention
    worker_id = 1  # w1

    # Lauching the job with a mocked WorkUS
    # The mocked server returns error_code 500 to DeployUS if
    # the json is not as expected
    expected_json = {"name":script_name, "file":filecontents.decode("utf-8")}
    workus1.expect_oneshot_request("/up", json=expected_json).respond_with_data("")

    # Sending requests to DeployUS to test its state
    response = DEPLOYUS.launch_job(script_id, worker_id)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 1

    # Testing entry in database
    assert dbjobs[0][0] == 1  # job id
    assert dbjobs[0][1] == script_id  # script id
    assert dbjobs[0][2] == worker_id  # worker id

    # Stopping the job with a mocked WorkUS
    # The mocked server returns error_code 500 to DeployUS if
    # the json is not as expected
    workus1.expect_oneshot_request("/down", json={"name":script_name}).respond_with_data("")

    # Sending requests to DeployUS to test its state
    response = DEPLOYUS.stop_job(job_id=1)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 0

    # Stopping the mocked WorkUS
    workus1.stop()


@pytest.mark.usefixtures("_db")
def test_launch_and_stop_job_normal_multiple():
    """
    Launching a job with two basic helloworld projects.
    Verifying with docker that the docker-compose files is up.

    Stopping projects afterwards as to not affect the other tests.
    """
    # Starting the mocked WorkUS
    workus1 = HTTPServer(host="workus1",port=WORKUS_PORT)
    workus2 = HTTPServer(host="workus2",port=WORKUS_PORT)
    workus1.start()
    workus2.start()

    # Inserting the workus worker #1
    resp = DEPLOYUS.insert_worker("w1", "workus1")
    dbworkers = DEPLOYUS.get_workers().json()
    assert resp.status_code == 200
    assert len(dbworkers) == 1

    # Inserting the workus worker #2
    resp = DEPLOYUS.insert_worker("w2", "workus2")
    dbworkers = DEPLOYUS.get_workers().json()
    assert resp.status_code == 200
    assert len(dbworkers) == 2

    # Inserting the hello-world script multiple times
    (script_name1, filecontents1), (script_name2, filecontents2) = test_insert_script_normal_multiple()

    # Setting up the expected responses on workus1 and workus2
    expected_json1 = {"name":script_name1, "file":filecontents1.decode("utf-8")}
    expected_json2 = {"name":script_name2, "file":filecontents2.decode("utf-8")}
    workus1.expect_oneshot_request("/up", json=expected_json1).respond_with_data("")
    workus2.expect_oneshot_request("/up", json=expected_json2).respond_with_data("")

    # Trying to launch both scripts on workus1
    response1 = DEPLOYUS.launch_job(script_id=1, worker_id=1)
    response2 = DEPLOYUS.launch_job(script_id=2, worker_id=1)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    # Fail as we can only launch one job on one worker at a time.
    assert response1.status_code == 200
    assert response2.status_code != 200
    assert len(dbjobs) == 1

    # Testing entries in database
    assert dbjobs[0][0] == 1  # job id
    assert dbjobs[0][1] == 1  # script id
    assert dbjobs[0][2] == 1  # worker id

    # Launching the second job on the second worker
    response = DEPLOYUS.launch_job(script_id=2, worker_id=2)
    dbjobs = DEPLOYUS.get_jobs().json()
    assert response.status_code == 200
    assert len(dbjobs) == 2
    assert dbjobs[0][0] == 1  # job id
    assert dbjobs[0][1] == 1  # script id
    assert dbjobs[0][2] == 1  # worker id
    assert dbjobs[1][0] == 2  # job id
    assert dbjobs[1][1] == 2  # script id
    assert dbjobs[1][2] == 2  # worker id

    # Setting up expected requests to workus1 and workus2
    workus1.expect_oneshot_request("/down", json={"name":script_name1}).respond_with_data("")
    workus2.expect_oneshot_request("/down", json={"name":script_name2}).respond_with_data("")

    # Stopping the jobs
    DEPLOYUS.stop_job(job_id=1)
    response = DEPLOYUS.stop_job(job_id=2)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 0

    # Stop mocked workus
    workus1.stop()
    workus2.stop()


@pytest.mark.usefixtures("_db")
def test_launch_job_bad_script_id():
    """
    A docker-compose file should not launch if the script does not
    exists in DeployUS
    """
    # Starting mocked workus1
    workus1 = HTTPServer(host="workus1",port=WORKUS_PORT)
    workus1.start()

    # Inserting the workus worker #1
    resp = DEPLOYUS.insert_worker("w1", "workus1")
    dbworkers = DEPLOYUS.get_workers().json()
    assert resp.status_code == 200
    assert len(dbworkers) == 1

    # Inserting the hello-world script
    (script_name, filecontents) = test_insert_script_normal()

    # Establishing expected results
    expected_json1 = {"name":script_name, "file":filecontents.decode("utf-8")}
    workus1.expect_oneshot_request("/up", json=expected_json1).respond_with_data("")

    # Attempting to launch script. Will fail as script id does not exists.
    script_id = 10  # This script doesn't exists. Therefore, the job should not launch
    worker_id = 1  # w1
    response = DEPLOYUS.launch_job(script_id, worker_id)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 422
    assert len(dbjobs) == 0

    # Closing mocked workus
    workus1.stop()

    
@pytest.mark.usefixtures("_db")
def test_cant_delete_a_running_script():
    """
    Launching a job with a basic helloworld project.
    The job launched opens on port 8000. We will send a GET request and check the html contents.

    Trying to delete the script from database while it is deployed should fail.

    Stopping project afterwards as to not affect the other tests.
    """
    # Starting mocked workus1
    workus1 = HTTPServer(host="workus1",port=WORKUS_PORT)
    workus1.start()

    # Inserting the workus worke
    resp = DEPLOYUS.insert_worker("w1", "workus1")
    dbworkers = DEPLOYUS.get_workers().json()
    assert resp.status_code == 200
    assert len(dbworkers) == 1

    # Inserting the hello-world script
    (script_name, filecontents) = test_insert_script_normal()
    script_id = 1  # Following MySQL AUTO_INCREMENT convention
    worker_id = 1  # w1

    # Establishing expected results
    expected_json1 = {"name":script_name, "file":filecontents.decode("utf-8")}
    workus1.expect_request("/up", json=expected_json1).respond_with_data("")
    
    # Launching the script
    response = DEPLOYUS.launch_job(script_id, worker_id)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 1

    # Testing entry in database
    assert dbjobs[0][0] == 1  # job id
    assert dbjobs[0][1] == script_id  # script id
    assert dbjobs[0][2] == worker_id  # worker id

    # Deleting script from the scripts table should fail as the script is running.
    response = DEPLOYUS.delete_script(script_id)
    dbscripts = DEPLOYUS.get_scripts().json()
    assert response.status_code == 422
    assert len(dbscripts) == 1

    # Establishing expected results for stopping the job
    workus1.expect_oneshot_request("/down", json={"name":script_name}).respond_with_data("")

    # Stopping the job
    response = DEPLOYUS.stop_job(job_id=1)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 0

    # Closing mocked workus
    workus1.stop()


@pytest.mark.usefixtures("_db")
def test_cant_delete_a_worker_with_a_running_script():
    """
    Launching a job with a basic helloworld project.
    The job launched opens on port 8000. We will send a GET request and check the html contents.

    Trying to delete the script from database while it is deployed should fail.

    Stopping project afterwards as to not affect the other tests.
    """
    # Starting mocked workus1
    workus1 = HTTPServer(host="workus1",port=WORKUS_PORT)
    workus1.start()

    # Inserting the workus worker
    resp = DEPLOYUS.insert_worker("w1", "workus1")
    dbworkers = DEPLOYUS.get_workers().json()
    assert resp.status_code == 200
    assert len(dbworkers) == 1

    # Inserting the hello-world script
    (script_name, filecontents) = test_insert_script_normal()
    script_id = 1  # Following MySQL AUTO_INCREMENT convention
    worker_id = 1  # w1

    # Establishing expected results
    expected_json1 = {"name":script_name, "file":filecontents.decode("utf-8")}
    workus1.expect_request("/up", json=expected_json1).respond_with_data("")

    # Launching the script on workus1
    response = DEPLOYUS.launch_job(script_id, worker_id)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 1

    # Testing entry in database
    assert dbjobs[0][0] == 1  # job id
    assert dbjobs[0][1] == script_id  # script id
    assert dbjobs[0][2] == worker_id  # worker id

    # Deleting worker should fail as it has a running script
    response = DEPLOYUS.delete_worker(worker_id)
    dbworkers = DEPLOYUS.get_workers().json()
    assert response.status_code == 422
    assert len(dbworkers) == 1

    # Establishing expected results for stopping the job
    workus1.expect_oneshot_request("/down", json={"name":script_name}).respond_with_data("")

    # Stopping the job
    response = DEPLOYUS.stop_job(job_id=1)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 200
    assert len(dbjobs) == 0

    # Closing mocked workus
    workus1.stop()


@pytest.mark.usefixtures("_db")
def test_launch_job_bad_worker_id():
    """
    A docker-compose file should not launch if the worker does not
    exists in DeployUS
    """
    # Starting mocked workus1
    workus1 = HTTPServer(host="workus1",port=WORKUS_PORT)
    workus1.start()

    # Inserting the workus worker #1
    resp = DEPLOYUS.insert_worker("w1", "workus1")
    dbworkers = DEPLOYUS.get_workers().json()
    assert resp.status_code == 200
    assert len(dbworkers) == 1

    # Inserting the hello-world script
    script_name, filecontents = test_insert_script_normal()
    script_id = 1
    worker_id = 10  # This worker doesn't exists. Therefore, the job should not launch

    # Establishing expected results
    expected_json1 = {"name":script_name, "file":filecontents.decode("utf-8")}
    workus1.expect_request("/up", json=expected_json1).respond_with_data("")

    # Attempting to launch script on workus
    response = DEPLOYUS.launch_job(script_id, worker_id)
    dbjobs = DEPLOYUS.get_jobs().json()

    # Testing reponse.
    assert response.status_code == 422
    assert len(dbjobs) == 0

    # Closing mocked workus
    workus1.stop()

@pytest.mark.usefixtures("_db")
def test_stop_job_dne():
    """
    Attempt to stop a job that does not exists.
    """
    # Starting mocked workus1
    workus1 = HTTPServer(host="workus1",port=WORKUS_PORT)
    workus1.start()

    # Establishing expected results for stopping the job
    workus1.expect_oneshot_request("/down").respond_with_data("")

    # Sends POST request to stop job.
    response = DEPLOYUS.stop_job(100)
    assert response.status_code == 422

    # Stopping workus
    workus1.stop()


