from app import app
from flask import render_template, make_response, jsonify
from flask import request, redirect
import json
import os
from . import db
from .utils import printus, allowed_file 
from werkzeug.utils import secure_filename

@app.route('/', methods=['GET'])
def index():
    return render_template('public/index.html',dbscripts=db.get_script())


@app.route('/insert_script', methods=['POST'])
def insert_script():
    if request.files:
        name = request.form.get('name')
        script = request.files["scriptfile"]

        if script.filename == "":
            return redirect('/')

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
            db.insert_script(name,binary_script)

            return redirect('/')

        else:
            printus("That file extension is not allowed")
            return redirect('/')
    else:
        return redirect('/')

@app.route('/delete_script', methods=['POST'])
def delete_script():
    # Deleting script
    id = request.get_json()['id']
    db.delete_script(id=id)

    return make_response("", 200)


@app.route('/launch', methods=['GET'])
def lauch():
    return render_template('public/launch.html',dbscripts=db.get_script(), dbjobs=db.get_jobs(), dbworkers=db.get_workers()) 

@app.route('/launch_job', methods=['POST'])
def launch_job():
    # Deleting script
    id = request.get_json()['id']
    location = request.get_json()['location']
    db.launch_job(id=id, location=location)

    return make_response("", 200)

@app.route('/stop_job', methods=['POST'])
def stop_job():
    # Deleting script
    job_id = request.get_json()['id']
    db.stop_job(job_id=job_id)

    return make_response("", 200)

@app.route('/workers', methods=['GET'])
def workers():
    return render_template('public/workers.html',dbworkers=db.get_workers()) 

@app.route('/insert_worker', methods=['POST'])
def insert_worker():
    name = request.get_json()['name']
    location = request.get_json()['location']
    db.insert_worker(name=name, location=location)

    return make_response("", 200)
    
@app.route('/delete_worker', methods=['POST'])
def delete_worker():
    id = request.get_json()['id']
    db.delete_worker(id=id)

    return make_response("", 200)