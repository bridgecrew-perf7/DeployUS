"""
DeployUS/run.py

Entry point to DeployUS.
Imports the flaskapp from the app/__init__.py file.

Run the flaskapp, which contains the infinite while loop.
    The host being set at 0.0.0.0 is standard in Flask.
"""

from app import flaskapp

if __name__ == "__main__":
    flaskapp.run(host="0.0.0.0")
