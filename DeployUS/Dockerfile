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

# Part0: must install docker-compose
RUN apt-get update
RUN apt-get install curl -y
RUN curl -L "https://github.com/docker/compose/releases/download/1.28.5/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
RUN chmod +x /usr/local/bin/docker-compose

# Executing DeployUS
CMD python run.py
