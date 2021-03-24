#!/bin/sh
echo $INDEX_TEXT > /usr/local/apache2/htdocs/index.html
cd /usr/local/bin/
exec httpd-foreground