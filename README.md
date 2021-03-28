# Testing different languages
## GO
To run, please use these following commands.  
Must have go installed on your device.
```
cd Golang
docker build -t my-golang-app .
docker run -it -p 8080:8080 --rm --name my-running-app my-golang-app
```

There is a hello-world example, an error example, and a json POST request example example.

## Python

