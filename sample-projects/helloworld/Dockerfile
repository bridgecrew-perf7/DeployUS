FROM ubuntu:latest as IndexFile
RUN echo "hello World" > index.html

FROM httpd:latest
COPY --from=IndexFile index.html /usr/local/apache2/htdocs/