FROM curlimages/curl:7.75.0 as curly
RUN curl -L "https://raw.githubusercontent.com/vishnubob/wait-for-it/master/wait-for-it.sh" -o ~/wait-for-it.sh
RUN chmod +x ~/wait-for-it.sh

# =========================================
# Using as we need the docker command for tests
FROM docker:20.10

# Installing python
RUN apk update
RUN apk add --no-cache py3-pip python3 bash docker-compose
RUN pip3 install --upgrade pip

# Copying tests files and wait-for-it.sh
COPY . /work
COPY --from=curly /home/curl_user/wait-for-it.sh /wait-for-it.sh

# Downloading dependancies
WORKDIR /work
RUN pip3 install -r requirements.txt 

CMD ["pytest",'-s','-v']
