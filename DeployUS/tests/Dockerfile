FROM curlimages/curl:7.75.0 as curly
RUN curl -L "https://raw.githubusercontent.com/vishnubob/wait-for-it/master/wait-for-it.sh" -o ~/wait-for-it.sh
RUN chmod +x ~/wait-for-it.sh

# =========================================
# Using python:3.8 and not python:3.8-slim as we need some standard modules
FROM python:3.8 
# Copying tests files and wait-for-it.sh
COPY . /work
COPY --from=curly /home/curl_user/wait-for-it.sh /wait-for-it.sh

# Downloading dependancies
WORKDIR /work
RUN pip install -r requirements.txt 

CMD ["pytest",'-s','-v']
