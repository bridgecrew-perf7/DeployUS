# Inspired from https://hub.docker.com/_/golang/
FROM golang:1.14-alpine

# Adding docker related packages
RUN apk add docker docker-compose

# Copying WorkUS files
WORKDIR /go/src/app
COPY main.go main.go

# Recommended by https://hub.docker.com/_/golang/
RUN go get -d -v ./...
RUN go install -v ./...

# Port as to which all communication will happen
# WorkUS will communicate through port 5002
EXPOSE 5002
CMD ["app"]