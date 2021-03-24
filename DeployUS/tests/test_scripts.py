"""
DeployUS/tests/test_scripts.py

Uses pytest to perform behavioral test of DeployUS's scripts related REST api.

To run:
    - python3 run_tests.py
"""
import hashlib
import pytest
from conftest import DeployUSInterface

DEPLOYUS = DeployUSInterface()

# =====================================================
#  Testing insertions/deletions of scripts
# =====================================================
@pytest.mark.usefixtures("_db")
def test_insert_script_normal():
    """
    Creating a hello world example docker-compose project
    and uploading it to DeployUS.

    Testing the state of the mysql database.
    """
    name = "myscript"
    filename = "docker-compose.yml"
    filecontents = b"""
        version: "3.3"
        services:
          app:
            image: shawnvosburg/helloworld:latest
            ports:
              - "8000:80"
    """

    # Testing hash of filecontents. This step ensures that the file transmitted wasn't corrupted.
    hash1 = hashlib.sha256(filecontents).hexdigest()

    # Sends POST request with file to DeployUS. Queries database for script table.
    response = DEPLOYUS.insert_script(name, filename, filecontents)
    dbscripts = DEPLOYUS.get_scripts().json()

    # Testing reponse. Not testing datetime uploaded because it is server specific.
    assert response.status_code == 200
    assert len(dbscripts) == 1
    assert dbscripts[0][0] == 1  # id
    assert dbscripts[0][1] == name  # name
    assert dbscripts[0][3] == hash1  # filehash


@pytest.mark.usefixtures("_db")
def test_insert_script_normal_multiple():
    """
    Creates two helloworld projects that open different ports (8000 and 8001 respectively).
    Their names are also different (myscript1 and myscript2 respectively).

    The file name is the standard docker-compose.yml.

    Uploads the scripts to DeployUS and queries database for sucessful upload.
    """
    name1 = "myscript1"
    name2 = "myscript2"
    filename = "docker-compose.yml"
    filecontents1 = b"""
        version: "3.3"
        services:
          app:
            image: shawnvosburg/helloworld:latest
            ports:
              - "8000:80"
    """
    filecontents2 = b"""
        version: "3.3"
        services:
          app:
            image: shawnvosburg/helloworld:latest
            ports:
              - "8001:80"
    """

    # Calcualte hash to compare with database to ensure
    # that the file has not been corrupted during upload.
    hash1 = hashlib.sha256(filecontents1).hexdigest()
    hash2 = hashlib.sha256(filecontents2).hexdigest()

    # Uploading scripts to DeployUS and querying state of database.
    DEPLOYUS.insert_script(name1, filename, filecontents1)
    response = DEPLOYUS.insert_script(name2, filename, filecontents2)
    dbscripts = DEPLOYUS.get_scripts().json()

    # Testing reponse. Not testing datetime uploaded because it is server specific.
    assert response.status_code == 200  # Successful reponse
    assert len(dbscripts) == 2  # The two scripts must be added
    assert dbscripts[0][0] == 1  # id myscript1
    assert dbscripts[0][1] == name1  # myscript1
    assert dbscripts[0][3] == hash1  # filehash of myscript1's script
    assert dbscripts[1][0] == 2  # id myscript2
    assert dbscripts[1][1] == name2  # myscript2
    assert dbscripts[1][3] == hash2  # filehash of myscript2's script


@pytest.mark.usefixtures("_db")
def test_insert_script_not_yaml():
    """
    Verifies that only .yml files can be uploaded.

    DeployUS should still return status_code 200.
    It should just not insert the script into the database.

    No warning should be given by DeployUS about the wrong file extension.
    Other than a 422 status code.
    """
    name = "myscript"
    filename = "scriptname.notyaml"
    filecontents = b"""
        version: "3.3"
        services:
          app:
            image: shawnvosburg/helloworld:latest
            ports:
              - "8000:80"
    """

    # Attempting to insert script and querying database for resulting state.
    response = DEPLOYUS.insert_script(name, filename, filecontents)
    dbscripts = DEPLOYUS.get_scripts().json()

    # Testing reponse. No file should be added to the database
    assert response.status_code == 422
    assert len(dbscripts) == 0


@pytest.mark.usefixtures("_db")
def test_insert_script_invalid_filecontents():
    """
    Verifies that only the file contents is valid.
    Valid file contents should pass the "docker-compose config" test
    """
    name = "myscript"
    filename = "scriptname.yml"
    # Note the invalid field.
    filecontents = b"""
        version: "3.3"
        InvalidField:
          app:
            image: shawnvosburg/helloworld:latest
            ports:
              - "8000:80"
    """

    # Attempting to insert script and querying database for resulting state.
    response = DEPLOYUS.insert_script(name, filename, filecontents)
    dbscripts = DEPLOYUS.get_scripts().json()

    # Testing reponse. No file should be added to the database.
    assert response.status_code == 422
    assert len(dbscripts) == 0


@pytest.mark.usefixtures("_db")
def test_insert_script_unique_name():
    """
    Verifying that a user can not give the same name
    to two different docker-compose scripts.
    """
    name = "myscript"
    filename1 = "docker-compose1.yml"
    filename2 = "docker-compose2.yml"
    filecontents1 = b"""
        version: "3.3"
        services:
          app:
            image: shawnvosburg/helloworld:latest
            ports:
              - "8000:80"
    """
    filecontents2 = b"""
        version: "3.3"
        services:
          app:
            image: shawnvosburg/helloworld:latest
            ports:
              - "8001:80"
    """

    # Adding script
    response = DEPLOYUS.insert_script(name, filename1, filecontents1)
    dbscripts = DEPLOYUS.get_scripts().json()

    # Testing reponse. Should be a success
    assert response.status_code == 200
    assert len(dbscripts) == 1

    # Adding script again
    response = DEPLOYUS.insert_script(name, filename2, filecontents2)
    dbscripts = DEPLOYUS.get_scripts().json()

    # Testing reponse. The script should not be added again as the name is the same
    assert response.status_code == 200
    assert len(dbscripts) == 1


@pytest.mark.usefixtures("_db")
def test_delete_script_normal():
    """
    Verifying that deleting a script works in DeployUS.

    An inserted script that has been deleted should not be in
    the database after deleted.
    """

    # Insert script into database
    test_insert_script_normal()
    dbscripts = DEPLOYUS.get_scripts().json()
    assert len(dbscripts) == 1

    # Send command to delete script with id 1 (AUTO_INCREMENT value of mysql)
    response = DEPLOYUS.delete_script(1)

    # Assertions that script table is empty
    dbscripts = DEPLOYUS.get_scripts().json()
    assert response.status_code == 200
    assert len(dbscripts) == 0
