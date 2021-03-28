# Testing different languages
## GO
To run, please use these following commands.  
Must have docker installed on your device.
```
cd Golang
docker build -t my-golang-app .
docker run -it -p 8080:8080 --rm my-golang-app
```

There is a hello-world example, an error example, and a json POST request example.

## Python
To run, please use these following commands.  
Must have docker installed on your device.
```
cd Python
docker build -t myflaskapp .
docker run -it -p 5000:5000 --rm myflaskapp
```

There is a hello-world example, an error example, and a json POST request example.