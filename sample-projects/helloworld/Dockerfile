FROM httpd:latest
COPY setup.sh /usr/local/bin/setup.sh
RUN chmod +x /usr/local/bin/setup.sh
WORKDIR /usr/local/bin
CMD ["setup.sh"]