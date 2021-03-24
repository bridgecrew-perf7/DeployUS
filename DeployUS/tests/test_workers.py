"""
DeployUS/tests/test_workers.py

Uses pytest to perform behavioral test of DeployUS's workers REST api.

To run:
    - python3 run_tests.py
"""
import pytest
from conftest import DeployUSInterface

DEPLOYUS = DeployUSInterface()


# =====================================================
#  Testing insertions/deletions of workers
# =====================================================
@pytest.mark.usefixtures("_db")
def test_insert_worker_normal():
    """
    Verifying that a worker and its address (known as location in DeployUS) can be added.
    """
    name = "worker1"
    location = "123.456.7.8"

    # Insertion through DeployUS and querying resultant state of database
    response = DEPLOYUS.insert_worker(name, location)
    dbworkers = DEPLOYUS.get_workers().json()

    # Testing reponse. Worker should be added
    assert response.status_code == 200
    assert len(dbworkers) == 2

    # Part 0: specifics
    assert dbworkers[0][0] == 1  # id
    assert dbworkers[0][1] == "localhost"  # name
    assert dbworkers[0][2] == "127.0.0.1"  # location

    # Verifying resultant state of database
    assert dbworkers[1][0] == 2  # id
    assert dbworkers[1][1] == name  # name
    assert dbworkers[1][2] == location  # location


@pytest.mark.usefixtures("_db")
def test_insert_worker_unique_name():
    """
    DeployUS should refuse the second insertion of
    workers being named the same.

    A 422 status code should be given as failure of insertion.
    """
    name = "worker1"
    location1 = "123.456.7.8"
    location2 = "987.654.3.2"

    # Adding two workers with the same name
    DEPLOYUS.insert_worker(name, location1)
    response = DEPLOYUS.insert_worker(name, location2)
    dbworkers = DEPLOYUS.get_workers().json()

    # Testing reponse. Worker should be added.
    assert response.status_code == 422
    assert len(dbworkers) == 2

    assert dbworkers[1][0] == 2  # id
    assert dbworkers[1][1] == name  # name
    assert dbworkers[1][2] == location1  #


@pytest.mark.usefixtures("_db")
def test_insert_worker_unique_location():
    """
    DeployUS should refuse the insertion of the second worker
    as the location is the same as the first

    A 422 status code should be given as failure of insertion.
    """
    name1 = "worker1"
    name2 = "worker2"
    location = "123.456.7.8"

    # Adding two workers with the same name
    DEPLOYUS.insert_worker(name1, location)
    response = DEPLOYUS.insert_worker(name2, location)
    dbworkers = DEPLOYUS.get_workers().json()

    # Testing reponse. Worker should be added.
    assert response.status_code == 422
    assert len(dbworkers) == 2
    assert dbworkers[1][0] == 2  # id
    assert dbworkers[1][1] == name1  # name
    assert dbworkers[1][2] == location  # location


@pytest.mark.usefixtures("_db")
def test_delete_worker_normal():
    """
    Inserting a new worker and testing that it can be deleted sucessfully.
    """
    # Inserting worker
    test_insert_worker_normal()

    # Deletion of worker and querying result with database
    response = DEPLOYUS.delete_worker(worker_id=2)
    dbworkers = DEPLOYUS.get_workers().json()

    # Testing reponse. Not testing datetime uploaded because it is server specific.
    assert response.status_code == 200
    assert len(dbworkers) == 1


@pytest.mark.usefixtures("_db")
def test_delete_worker_non_existant():
    """
    The deletion of worker that does not exists should return a
    200 status code as the final state of the database is that the worker is not present.
    """
    # Inserting worker
    test_insert_worker_normal()

    response = DEPLOYUS.delete_worker(worker_id=10)  # This id does not exists in db
    dbworkers = DEPLOYUS.get_workers().json()

    # Testing reponse. Not testing datetime uploaded because it is server specific.
    assert response.status_code == 200
    assert len(dbworkers) == 2
