import mysql.connector

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
def get_script(cursor, connection):
    cursor.execute('SELECT * FROM scripts')
    results = [(name, str(date), str(contents)) for (name, date, contents) in cursor]

    return results

@mysql_safe
def insert_script(cursor, connection,name_,contents):
    sql = 'INSERT INTO scripts (name, cre_date, contents) VALUES (%s, %s, %s );'
    val = (name_, '2021-02-02', str(contents))
    cursor.execute(sql,val)
    connection.commit()