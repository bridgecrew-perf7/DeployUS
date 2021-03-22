FROM ubuntu:latest as IndexFile
RUN echo "Hello World!" > index.html

FROM httpd:latest
COPY --from=IndexFile index.html /usr/local/apache2/htdocs/