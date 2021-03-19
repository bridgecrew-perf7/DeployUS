from app import app
from flask import render_template
from flask import request, redirect
import mysql.connector
import json
import sys
import os
import datetime
from werkzeug.utils import secure_filename

def printus(thingToPrint):
    print(thingToPrint, file=sys.stdout)

def get_script():
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
    results = [(name, str(date), str(contents)) for (name, date, contents) in cursor]
    cursor.close()
    connection.close()

    return results

def insert_script(name_,contents):
    config = {
        'user': 'root',
        'password': 'deployus',
        'host': 'db',
        'port': '3306',
        'database': 'deployusdb'
    }
    connection = mysql.connector.connect(**config)
    cursor = connection.cursor()
    sql = 'INSERT INTO scripts (name, cre_date, contents) VALUES (%s, %s, %s );'
    val = (name_, '2021-02-02', str(contents))
    cursor.execute(sql,val)
    connection.commit()
    cursor.close()
    connection.close()

def allowed_file(filename):
    # Must have at least 1 . in filename
    if not "." in filename:
        return False

    # Check if the extension is in ALLOWED_IMAGE_EXTENSIONS
    ext = filename.rsplit(".", 1)[1]
    if ext.upper() == 'YML':
        return True
    else:
        return False

@app.route('/', methods=['GET', 'POST'])
def index():
    # Posting requires uploading to database
    if request.method == "POST":
        if request.files:
            name = request.form.get('name')
            script = request.files["scriptfile"]

            if script.filename == "":
                return redirect(request.url)

            if allowed_file(script.filename):
                filename = secure_filename(script.filename)

                # Inserting script in database.
                # 1. Save in temp path to reload :) 
                path = os.path.join('app','static','uploads',filename)
                script.save(path)

                # 2. Load in bytes format
                with open(path,'rb') as file:
                    binary_script = file.read()
                
                # 3. Upload to the database
                insert_script(name,binary_script)

                return redirect(request.url)

            else:
                printus("That file extension is not allowed")
                return redirect(request.url)

    return render_template('public/index.html')

@app.route('/scripts')
def scripts():
    return json.dumps({'scripts':get_script()})