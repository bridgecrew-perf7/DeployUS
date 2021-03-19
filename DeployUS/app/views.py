from app import app
from flask import render_template
from flask import request, redirect
import json
import os
from . import db
from .utils import printus, allowed_file 
from werkzeug.utils import secure_filename

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
                db.insert_script(name,binary_script)

                return redirect(request.url)

            else:
                printus("That file extension is not allowed")
                return redirect(request.url)

    return render_template('public/index.html')

@app.route('/scripts')
def scripts():
    import pprint
    return pprint.pformat(json.dumps({'scripts': db.get_script()}), indent=4)

@app.route('/execute', methods=['GET'])
def execute():
    db.execute_script("myscript")
    return render_template('public/index.html') 