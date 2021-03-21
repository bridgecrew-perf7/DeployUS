FROM curlimages/curl:7.75.0 as dockercomposeinstall
RUN curl -L "https://github.com/docker/compose/releases/download/1.28.5/docker-compose-$(uname -s)-$(uname -m)" -o ~/docker-compose
RUN curl -L "https://raw.githubusercontent.com/vishnubob/wait-for-it/master/wait-for-it.sh" -o ~/wait-for-it.sh
RUN chmod +x ~/docker-compose
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
COPY run.py /work
COPY requirements.txt /work

# DeployUS specific dependencies
WORKDIR /work
RUN pip install -r requirements.txt

# Part 0: Copy docker-compose from curl stage
COPY --from=dockercomposeinstall /home/curl_user/docker-compose /usr/local/bin/docker-compose
COPY --from=dockercomposeinstall /home/curl_user/wait-for-it.sh /wait-for-it.sh

# Executing DeployUS
CMD python run.py