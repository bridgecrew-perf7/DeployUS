# Using python's official image
FROM python:3.8-slim

# All REST messages will be sent through this port.
EXPOSE 5000

# Flask-specific environment variable
ENV FLASK_APP=run.py
ENV FLASK_ENV=development
ENV PYTHONUNBUFFERED=1

# The app will be contained with the /work directory on the container
COPY app/ /work/app
COPY run.py /work
COPY requirements.txt /work

# DeployUS specific dependencies
WORKDIR /work
RUN pip install -r requirements.txt

# Executing DeployUS
CMD python run.py
