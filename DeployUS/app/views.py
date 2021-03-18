from app import app
import mysql.connector
import json

def get_scripts():
    config = {
        'user': 'root',
        'password': 'deployus',
        'host': 'db',
        'port': '3306',
        'database': 'deployusdb'
    }
    connection = mysql.connector.connect(**config)
    cursor = connection.cursor()
    cursor.execute('SELECT * FROM scripts')
    results = [({name: path}, str(cre_date)) for (name, path, cre_date) in cursor]
    cursor.close()
    connection.close()

    return results

@app.route('/')
def index():
    return json.dumps({'scripts': get_scripts()})

@app.route("/about")
def about():
    return "<h1 style='color:red'>About!!</h1>"    
