"""
DeployUS/app/db.py

Modules that wraps the MySQL database specific to the DeployUS application.
"""
import os
import pathlib
import json
import mysql.connector
from app import utils

# This ensures that the path is not hard-coded to where the script is executed.
DB_CONFIG_FILE = pathlib.Path(__file__).parent.parent.joinpath("dbconfig.json")


def mysql_safe(func):
    """
    Decorator to safely connect/disconnect from database
    """

    def inner(*args, **kwargs):
        """
        Creates mysql-connector objects and releases them after the function
        is executed
        """
        with open(DB_CONFIG_FILE) as config_file:
            config = json.load(config_file)
        connection = mysql.connector.connect(**config)
        cursor = connection.cursor()

        # Execute function
        result = func(*args, cursor=cursor, connection=connection, **kwargs)

        cursor.close()
        connection.close()

        return result

    return inner


# ========================================================
#   Interface with database
# ========================================================


@mysql_safe
def get_scripts(**kwargs):
    """
    Retrieve all data in the scripts table
    Parameters:
        kwargs:
            cursor: mysql-connector cursor. Executes SQL commands.
    Returns:
        List of [script_id, script_name, script_date, script_hash]
    """
    cursor = kwargs["cursor"]
    cursor.execute("SELECT * FROM scripts")
    results = [
        (id, name, utils.format_datetime_obj(str(date)), utils.get_hash(contents))
        for (id, name, date, contents) in cursor
    ]

    return results


@mysql_safe
def get_jobs_verbose(**kwargs):
    """
    Retrieves jobs with script name and worker location.

    Returns:
        list of [job_id, script_name, worker_location]
    """
    cursor = kwargs["cursor"]
    sql = """
        SELECT j.id, s.name, w.location
        FROM jobs AS j
        JOIN scripts AS s ON j.script_id = s.id
        JOIN workers AS w on j.worker_id = w.id;
    """
    cursor.execute(sql)
    results = list(cursor)

    return results


@mysql_safe
def get_jobs(**kwargs):
    """
    Retrieves jobs with script id and worker id.

    Returns:
        list of [job_id, script_id, worker_id, launch_date]
    """
    cursor = kwargs["cursor"]
    sql = """
        SELECT *
        FROM jobs;
    """
    cursor.execute(sql)
    results = [
        (job_id, script_id, worker_id, utils.format_datetime_obj(str(date)))
        for (job_id, script_id, worker_id, date) in cursor
    ]

    return results


@mysql_safe
def get_workers(**kwargs):
    """
    Retrieves workers information

    Returns:
        list of [worker_id, worker_name, worker_location]
    """
    cursor = kwargs["cursor"]
    cursor.execute("SELECT * FROM workers")
    results = list(cursor)
    return results


@mysql_safe
def insert_worker(**kwargs):
    """
    Attempts to insert new worker in table workers.
    Must give name and location of worker.
    Parameters:
        kwargs:
            name: string. Alias of worker.
            location: string. IP address to communicate with worker.
    Returns:
        True if successful. False if an error occured.
    """
    cursor = kwargs["cursor"]
    connection = kwargs["connection"]
    name = kwargs["name"]
    location = kwargs["location"]
    sql = "INSERT INTO workers (name,location) VALUES (%s, %s);"
    val = (name, location)
    # There might be an error due to unique keys
    try:
        cursor.execute(sql, val)
        connection.commit()
        return True
    except mysql.connector.errors.IntegrityError:
        return False


@mysql_safe
def insert_script(**kwargs):
    """
    Attempts to insert new script in table scripts.
    Must give name and filecontents of scripts.
    Parameters:
        kwargs:
            name: string. Alias of script.
            contents: string. File contents of script contents.
    Returns:
        True if successful. False if an error occured.
    """
    # Parse kwargs to get arguments.
    cursor = kwargs["cursor"]
    connection = kwargs["connection"]
    name = kwargs["name"]
    contents = kwargs["contents"]

    # Executes MySQL command
    sql = "INSERT INTO scripts (name, cre_date, contents) VALUES (%s, %s, %s );"
    val = (name, utils.get_datetime_now(), contents)

    # There might be an error due to unique keys
    try:
        cursor.execute(sql, val)
        connection.commit()
        return True
    except mysql.connector.errors.IntegrityError:
        return False


@mysql_safe
def delete_worker(**kwargs):
    """
    Deletes worker from table workers.
    Parameters:
        kwgars:
            id: Unique identifyer under the 'id' column in the MySQL workers table.
    Returns:
        Nothing.
    """
    cursor = kwargs["cursor"]
    connection = kwargs["connection"]
    id_ = kwargs["id"]
    sql = f"DELETE FROM workers WHERE id = {id_};"
    cursor.execute(sql)
    connection.commit()


@mysql_safe
def delete_script(**kwargs):
    """
    Deletes script from table scripts.
    Parameters:
        kwgars:
            id: Unique identifyer under the 'id' column in the MySQL scripts table.
    Returns:
        Nothing.
    """
    cursor = kwargs["cursor"]
    connection = kwargs["connection"]
    id_ = kwargs["id"]
    sql = f"DELETE FROM scripts WHERE id = {id_};"
    cursor.execute(sql)
    connection.commit()


@mysql_safe
def launch_job(**kwargs):
    """
    Lauch jobs with a script and worker in database.
    Parameters:
        kwgars:
            _script_id: Unique identifyer under the 'id' column in the MySQL scripts table.
            _worker_id: Unique identifyer under the 'id' column in the MySQL workers table.
    Returns:
        True if successful. False otherwise.
    """
    cursor = kwargs["cursor"]
    connection = kwargs["connection"]
    script_id = kwargs["script_id"]
    worker_id = kwargs["worker_id"]

    # Verifying that script exists
    cursor.execute(f"SELECT * FROM scripts WHERE id = '{script_id}'")
    results = list(cursor)
    if len(results) == 0:
        return False
    (script_id, script_name, _, contents) = results[0]

    # Verifying that worker exists
    cursor.execute(f"SELECT * FROM workers WHERE id = '{worker_id}'")
    results = list(cursor)
    if len(results) == 0:
        return False
    (worker_id, _, _) = results[0]

    # Write file to disk
    parentdir = f"/work/scripts/{script_name}"
    dockercompose_path = os.path.join(parentdir, "docker-compose.yml")
    if not os.path.exists(parentdir):
        os.makedirs(parentdir)

    with open(dockercompose_path, "wb") as file:
        file.write(contents)

    # docker-compose execution. Return False if failure to bring docker-compose up.
    cmd = f"cd {parentdir};  docker-compose pull --ignore-pull-failures "
    if os.system(cmd):
        return False
    cmd = f"cd {parentdir};  docker-compose up -d"
    if os.system(cmd):
        return False

    # Insert job's location into jobs table in db
    sql = "INSERT INTO jobs (script_id, worker_id, launch_date) VALUES (%s, %s, %s);"
    val = (script_id, worker_id, utils.get_datetime_now())
    cursor.execute(sql, val)
    connection.commit()
    return True


@mysql_safe
def stop_job(**kwargs):
    """
    Stops a job that is running.
    Parameters:
        kwargs:
            job_id: int. Unique identifyer of the job.
    Returns:

    """
    cursor = kwargs["cursor"]
    connection = kwargs["connection"]
    job_id = kwargs["job_id"]

    sql = f"""
        SELECT j.id, s.name, w.location
        FROM jobs AS j
        JOIN scripts AS s ON j.script_id = s.id
        JOIN workers AS w on j.worker_id = w.id
        WHERE j.id = {job_id};
    """
    cursor.execute(sql)
    results = list(cursor)

    # Script does not exists
    if len(results) == 0:
        return False
    (_, script_name, _) = results[0]

    # docker compose execution
    parentdir = f"/work/scripts/{script_name}"
    cmd = f"cd {parentdir};  docker-compose down"
    os.system(cmd)

    # Insert job's location into jobs table in db
    sql = f"DELETE FROM jobs WHERE id = {job_id}"
    cursor.execute(sql)
    connection.commit()
    return True
