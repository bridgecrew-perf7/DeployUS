from app import flaskapp
from flask import render_template, make_response, jsonify
from flask import request, redirect
import os
from . import db
from .utils import printus, allowed_file 
from werkzeug.utils import secure_filename

@flaskapp.route('/', methods=['GET'])
def index():
    return render_template('public/index.html',dbscripts=db.get_scripts())

@flaskapp.route('/get_scripts', methods=['GET'])
def get_scripts():
    return jsonify(db.get_scripts())

@flaskapp.route('/get_jobs', methods=['GET'])
def get_jobs():
    return jsonify(db.get_jobs())

@flaskapp.route('/get_workers', methods=['GET'])
def get_workers():
    return jsonify(db.get_workers())

@flaskapp.route('/insert_script', methods=['POST'])
def insert_script():
    if request.files:
        name = request.form.get('name')
        script = request.files["scriptfile"]

        if script.filename == "":
            return redirect('/')

        if allowed_file(script.filename):
            filename = secure_filename(script.filename)

            # Inserting script in database.
            # 1. Save in temp path to reload and validate :) 
            path = os.path.join('app','static','uploads',filename)
            script.save(path)

            # 2. Validate with docker-compose config -q 
            if os.system(f"docker-compose -f {path} config -q"):
                printus("Not a valid docker-compose file.")
                return redirect('/',code=422)

            # 3. Load in bytes format
            with open(path,'rb') as file:
                binary_script = file.read()
            
            # 4. Upload to the database
            db.insert_script(name,binary_script)

            return redirect('/')

        else:
            printus("That file extension is not allowed")
            printus(script.filename)
            return redirect('/',code=422)
    else:
        return redirect('/',code=422)

@flaskapp.route('/delete_script', methods=['POST'])
def delete_script():
    # Deleting script
    id = request.get_json()['id']
    db.delete_script(id=id)

    return make_response("", 200)


@flaskapp.route('/launch', methods=['GET'])
def lauch():
    return render_template('public/launch.html',dbscripts=db.get_scripts(), dbjobs=db.get_jobs_verbose(), dbworkers=db.get_workers()) 

@flaskapp.route('/launch_job', methods=['POST'])
def launch_job():
    # Deleting script
    script_id = request.get_json()['id']
    worker_id = request.get_json()['location']
    if db.launch_job(_script_id=script_id, _worker_id=worker_id):
        return make_response("", 200)
    else:
        return make_response("", 422)

@flaskapp.route('/stop_job', methods=['POST'])
def stop_job():
    # Deleting script
    job_id = request.get_json()['id']
    db.stop_job(job_id=job_id)

    return make_response("", 200)

@flaskapp.route('/workers', methods=['GET'])
def workers():
    return render_template('public/workers.html',dbworkers=db.get_workers()) 

@flaskapp.route('/insert_worker', methods=['POST'])
def insert_worker():
    name = request.get_json()['name']
    location = request.get_json()['location']
    if db.insert_worker(name=name, location=location):
        return make_response("", 200)
    else:
        return make_response("", 422)

    
    
@flaskapp.route('/delete_worker', methods=['POST'])
def delete_worker():
    id = request.get_json()['id']
    db.delete_worker(id=id)

    return make_response("", 200)