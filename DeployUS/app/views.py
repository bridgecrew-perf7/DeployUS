"""
DeployUS/app/views.py

Defines all REST api endpoints of DeployUS
"""
import os
from flask import render_template, make_response, jsonify
from flask import request, redirect
from flask import Flask
from werkzeug.utils import secure_filename
from . import db

# Creating the flask app as a global object
flaskapp = Flask('app')

@flaskapp.route('/', methods=['GET'])
def index():
    """
    Renders page of / endpoint.
    """
    return render_template('public/index.html',dbscripts=db.get_scripts())

@flaskapp.route('/get_scripts', methods=['GET'])
def get_scripts():
    """
    Returns all contents of scripts table.
    """
    return jsonify(db.get_scripts())

@flaskapp.route('/get_jobs', methods=['GET'])
def get_jobs():
    """
    Returns all contents of jobs table.
    """
    return jsonify(db.get_jobs())

@flaskapp.route('/get_workers', methods=['GET'])
def get_workers():
    """
    Returns all contents of workers table.
    """
    return jsonify(db.get_workers())

@flaskapp.route('/insert_script', methods=['POST'])
def insert_script():
    """
    Attempts to insert script in database.
    The script is passed as a multi-form element.

    The name is in the form section and the
    scriptfile is in the files sections of the requests.

    Reponds with status_code 200 if sucessfull,
                 status_code 422 if an error occured.
    In all cases, redirects to / endpoint.
    """
    if request.files:
        name = request.form.get('name')
        script = request.files["scriptfile"]

        if script.filename == "":
            return redirect('/')

        if script.filename.endswith('.yml'):
            filename = secure_filename(script.filename)

            # Inserting script in database.
            # 1. Save in temp path to reload and validate :)
            path = os.path.join('app','static','uploads',filename)
            script.save(path)

            # 2. Validate with docker-compose config -q
            if os.system(f"docker-compose -f {path} config -q"):
                print("Not a valid docker-compose file.")
                return redirect('/',code=422)

            # 3. Load in bytes format
            with open(path,'rb') as file:
                binary_script = file.read()

            # 4. Upload to the database
            db.insert_script(name=name,contents=binary_script)

            return redirect('/')

        # File extension does not end with .yml
        print("That file extension is not allowed")
        print(script.filename)
        return redirect('/',code=422)

    # No files were provided.
    return redirect('/',code=422)

@flaskapp.route('/delete_script', methods=['POST'])
def delete_script():
    """
    Deletes script by id. The POST request must be made with
    id set in the json field.
    """
    # Deleting script
    id_ = request.get_json()['id']
    db.delete_script(id=id_)

    return make_response("", 200)


@flaskapp.route('/launch', methods=['GET'])
def lauch():
    """
    Renders the /launch endpoint.
    """
    return render_template('public/launch.html',
                            dbscripts=db.get_scripts(),
                            dbjobs=db.get_jobs_verbose(),
                            dbworkers=db.get_workers())

@flaskapp.route('/launch_job', methods=['POST'])
def launch_job():
    """
    Launch a job with a script_id ans a worker_id.
    The post requests must be sent with the fields:
        id = script_id
        location = worker_id

    Returns 200 if sucessful, 422 if an error occured.
    """
    # Deleting script
    script_id = request.get_json()['id']
    worker_id = request.get_json()['location']
    if db.launch_job(script_id=script_id, worker_id=worker_id):
        return make_response("", 200)

    # The job did not launch.
    return make_response("", 422)

@flaskapp.route('/stop_job', methods=['POST'])
def stop_job():
    """
    Attempts to stop running jobs
    The post requests must be sent with the fields:
        id = job_id

    Returns 200 if sucessful, 422 if an error occured.
    """
    job_id = request.get_json()['id']
    if db.stop_job(job_id=job_id):
        return make_response("", 200)

    # The job did not stop.
    return make_response("", 422)

@flaskapp.route('/workers', methods=['GET'])
def workers():
    """
    Renders the /workers endpoints.
    """
    return render_template('public/workers.html',dbworkers=db.get_workers())

@flaskapp.route('/insert_worker', methods=['POST'])
def insert_worker():
    """
    Attempts to insert a new worker
    The post requests must be sent with the fields:
        name = worker alias
        location = ip address of the worker

    Returns 200 if sucessful, 422 if an error occured.
    """
    name = request.get_json()['name']
    location = request.get_json()['location']
    if db.insert_worker(name=name, location=location):
        return make_response("", 200)

    # The worker was not inserted.
    return make_response("", 422)

@flaskapp.route('/delete_worker', methods=['POST'])
def delete_worker():
    """
    Attempts to delete a worker
    The post requests must be sent with the fields:
        id = worker id

    Returns 200 status_code
    """
    id_ = request.get_json()['id']
    db.delete_worker(id=id_)

    return make_response("", 200)
