FROM curlimages/curl:7.75.0 as curly
RUN curl -L "https://raw.githubusercontent.com/vishnubob/wait-for-it/master/wait-for-it.sh" -o ~/wait-for-it.sh
RUN chmod +x ~/wait-for-it.sh


# ====================================================

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
COPY db/dbconfig.json /work/dbconfig.json
COPY run.py /work
COPY requirements.txt /work

# DeployUS specific dependencies
WORKDIR /work
RUN pip install -r requirements.txt

# Load the wait-fot-it.sh script
COPY --from=curly /home/curl_user/wait-for-it.sh /wait-for-it.sh

# Executing DeployUS
CMD python run.py