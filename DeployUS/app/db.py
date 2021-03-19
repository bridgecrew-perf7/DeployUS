import mysql.connector
import os

def mysql_safe(func):
    """
        Decorator to safely connect/disconnect from database
    """
    def inner(*args, **kwargs):
        config = {
        'user': 'root',
        'password': 'deployus',
        'host': 'db',
        'port': '3306',
        'database': 'deployusdb'
        }
        connection = mysql.connector.connect(**config)
        cursor = connection.cursor()

        # Execute function
        result = func(cursor, connection, *args, **kwargs)

        cursor.close()
        connection.close()

        return result
    return inner


# ========================================================
#   Interface with database
# ========================================================

@mysql_safe
def get_script(cursor=None, connection=None):
    cursor.execute('SELECT * FROM scripts')
    results = [(name, str(date), str(contents)) for (name, date, contents) in cursor]

    return results

@mysql_safe
def insert_script(cursor, connection, name_, contents):
    sql = 'INSERT INTO scripts (name, cre_date, contents) VALUES (%s, %s, %s );'
    val = (name_, '2021-02-02', contents)
    cursor.execute(sql,val)
    connection.commit()

@mysql_safe
def execute_script(cursor, connection, name_):
    cursor.execute(f"SELECT * FROM scripts WHERE name = '{name_}'")
    results = [(name, str(date), contents) for (name, date, contents) in cursor]

    # Script does not exists
    if len(results) == 0:
        return
    (name, date, contents) = results[0]

    # Write file to disk
    os.mkdir(name)
    with open(f"{name}/docker-compose.yml", 'wb') as file:
        file.write(contents)

    # Docker in Docker execution
    
