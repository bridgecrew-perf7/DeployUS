from flask import Flask, make_response, request

flaskapp = Flask(__name__)

# Hello world example
@flaskapp.route("/")
def index():
    html = "<p>HELLO WORLD! Please visit:<br>"
    html += "http://localhost:5000/error<br>"
    html += "http://localhost:5000/json</p>"
    return html

# 422 error example
@flaskapp.route("/error")
def errorFunc():
    return make_response("Check in web console for status code. Should be 422", 422)

# JSON example
@flaskapp.route("/json", methods=["GET","POST"])
def json_func():
    if request.method == 'GET':
        return """Couldn't understand the JSON body. If cURL is available, use command:<br>
		curl -X POST -H "Content-Type: application/json" -d '{"text":"json body"}' http://localhost:5000/json"""

    # Fetching json contents
    jsonBody = request.get_json()

    # Sending back its contents
    return make_response(f"The json received is: {jsonBody}", 200)