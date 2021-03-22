"""
DeployUS/tests/conftest.py

Creates useful classes and pytest fixtures to test DeployUS.
"""
import requests
import pytest
import mysql.connector

class DataBase():
    """
    Class that wraps the MySQL database with config specifics to DeployUS
    """
    def __init__(self):
        """
        Connects to the MySQL database.
        """
        config = {
            'user': 'root',
            'password': 'deployus',
            'host': 'db',
            'port': '3306',
            'database': 'deployusdb'
        }
        self.connection = mysql.connector.connect(**config)
        self.cursor = self.connection.cursor()

    def query(self,query):
        """
        Standarizes a SQL query.

        Parameter:
            query: string. A valid SQL query
        """
        # Send command. Must set argument if multiple querys (marked by semi-colon)
        multi = query.count(';') > 1
        self.cursor.execute(query, multi=multi)

        # Fetch results, if any
        results =  list(self.cursor)

        # Commit to the mysql connection
        self.connection.commit()

        # Return
        return results

    def clear_and_close(self):
        """
        Cleanly deletes everything in the SQL tables and resets the AUTO_INCREMENT values.
        Performs some assertions to see that the database has been cleared.

        Finally, safely closes the MySQL connections.
        """
        # Clearing database for next test
        self.query("DELETE FROM jobs;")
        self.query("DELETE FROM scripts;")
        self.query("DELETE FROM workers;")

        # Resetting the id AUTO_INCREMENT values
        self.query("ALTER TABLE jobs AUTO_INCREMENT = 1;")
        self.query("ALTER TABLE scripts AUTO_INCREMENT = 1;")
        self.query("ALTER TABLE workers AUTO_INCREMENT = 1;")

        # Assertions that all tables are empty
        assert len(self.query("SELECT * FROM jobs;")) == 0
        assert len(self.query("SELECT * FROM scripts;")) == 0
        assert len(self.query("SELECT * FROM workers;")) == 0

        # Part 0 specifics
        self.query("INSERT INTO workers (name, location) VALUES ('localhost','127.0.0.1');")
        assert len(self.query("SELECT * FROM workers;")) == 1

        # Close connection
        self.connection.close()
        self.cursor.close()

class DeployUSInterface():
    """
    Useful class that wraps REST endpoints under test.
    """
    def __init__(self):
        """
        Defines the location and port of DeployUS.
        Assumes that the docker-compose file has the
        DeployUS service named as app.
        """
        self.addr = 'http://app:5000'

    def index_page(self):
        """
        Sends GET request to the index page
        Returns:
          response to the requests.
        """
        return requests.get(self.addr + '/')

    def workers_page(self):
        """
        Sends GET request to the workers page
        Returns:
          response to the requests.
        """
        return requests.get(self.addr + '/workers')

    def launch_page(self):
        """
        Sends GET request to the launch page
        Returns:
          response to the requests.
        """
        return requests.get(self.addr + '/launch')

    def get_scripts(self):
        """
        Sends GET request to the /get_scripts endpoint
        Returns:
          response to the requests.
        """
        return requests.get(self.addr + '/get_scripts')

    def get_jobs(self):
        """
        Sends GET request to the /get_jobs endpoint
        Returns:
          response to the requests.
        """
        return requests.get(self.addr + '/get_jobs')

    def get_workers(self):
        """
        Sends GET request to the /get_workers endpoint
        Returns:
          response to the requests.
        """
        return requests.get(self.addr + '/get_workers')

    def insert_script(self, script_name, script_filename, script_filecontents):
        """
        Sends POST request to the /insert_script endpoint

        Parameters:
          script_name: str. Alias of docker-compose in DeployUS.
          script_filename: str. Filename of docker-compose.yml
          script_filecontents: str. Verbatim binary contents of docker-compose file.
        Returns:
          response to the requests.
        """
        files = {'name': (None,script_name), 'scriptfile': (script_filename, script_filecontents)}
        response = requests.post(self.addr + '/insert_script', files=files)
        return response

    def delete_script(self, script_id):
        """
        Sends POST request to the /delete_script endpoint

        Parameters:
          script_id: int. Index of script to delete from database
        """
        payload = {'id': script_id}
        response = requests.post(self.addr + '/delete_script', json=payload)
        return response

    def launch_job(self, script_id, worker_id):
        """
        Sends POST request to the /launch_job endpoint

        Parameters:
          script_id: int. ID of script to launch
          worker_id: int. ID of the worker to launch from.
        Returns:
          response to the request.
        """
        payload = {'id': script_id, 'location':worker_id}
        response = requests.post(self.addr + '/launch_job', json=payload)
        return response

    def stop_job(self, job_id):
        """
        Sends POST request to the /stop_job endpoint

        Parameters:
          job_id: int. ID of job to terminate.
        Returns:
          response to the request.
        """
        payload = {'id': job_id}
        response = requests.post(self.addr + '/stop_job', json=payload)
        return response

    def insert_worker(self, name, location):
        """
        Sends POST request to the /insert_worker endpoint

        Parameters:
          name: str. Alias of the worker
          location: str. IP address of the worker. Will be used to send files over.
        Returns:
          response to the request.
        """
        payload = {'name': name, 'location': location}
        response = requests.post(self.addr + '/insert_worker', json=payload)
        return response

    def delete_worker(self, worker_id):
        """
        Sends POST request to the /delete_worker endpoint

        Parameters:
          worker_id: int. ID of the worker to launch from.
        Returns:
          response to the request.
        """
        payload = {'id': worker_id}
        response = requests.post(self.addr + '/delete_worker', json=payload)
        return response


# Takes care of setup & teardown of database interface
@pytest.fixture(autouse=True)
def _db():
    """
    Pytest Fixture _db.
    This fixture setups and teardowns the database.

    This is used to reset the database to a clean state
    so that we don't have to recreate the database after
    each test. Greatly speeds up integration tests.
    """
    # Create DataBase object
    db_ = DataBase()

    # Perform test
    yield

    # Teardown
    db_.clear_and_close()
