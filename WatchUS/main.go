package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"os/exec"
)

type watchable struct {
	File string `json:"file"`
}

func main() {
	http.HandleFunc("/up", dockerComposeUp)
	http.HandleFunc("/down", dockerComposeDown)

	// DeployUS is on port 5000, WatchUS will be on port 5001
	http.ListenAndServe(":5001", nil)
}

func dockerComposeUp(writer http.ResponseWriter, reqest *http.Request) {
	// Initialize empty toWatch
	var toWatch watchable

	// Parse request for the json which contains the file
	body, _ := ioutil.ReadAll(reqest.Body)
	errJSON := json.Unmarshal([]byte(body), &toWatch)
	if errJSON != nil {
		writer.WriteHeader(http.StatusUnprocessableEntity)
		writer.Write([]byte("Couldn't understand the JSON body."))
		return
	}
	defer reqest.Body.Close()

	// Make sure all parent directories exists
	parentDir := "/work"
	_, err := os.Stat(parentDir)
	if os.IsNotExist(err) {
		mdkirErr := os.MkdirAll(parentDir, 0700) //All permission to user
		if mdkirErr != nil {
			writer.WriteHeader(http.StatusInternalServerError)
			errString := fmt.Sprintf("Could not create the %s directory. %s", parentDir, mdkirErr)
			writer.Write([]byte(errString))
			return
		}
	}

	// Write the file to disk at /work/docker-compose.yml
	file, err := os.Create("/work/docker-compose.yml")
	file.WriteString(toWatch.File)
	if err != nil {
		writer.WriteHeader(http.StatusInternalServerError)
		writer.Write([]byte("Could not create the /work/docker-compose.yml."))
		return
	}

	// Pull necessary images
	cmdArgs := []string{"-f", "/work/docker-compose.yml", "pull", "--ignore-pull-failures"}
	_, err = exec.Command("docker-compose", cmdArgs...).Output()
	if err != nil {
		writer.WriteHeader(http.StatusInternalServerError)
		writer.Write([]byte("Could not pull the docker images."))
		return 
	}

	// Run in detach mode.
	cmdArgs = []string{"-f", "/work/docker-compose.yml", "up", "-d", "--force-recreate"}
	_, err = exec.Command("docker-compose", cmdArgs...).Output()

	// Catch all errors, useful for CI
	if err != nil {
		writer.WriteHeader(http.StatusInternalServerError)
		writer.Write([]byte("Could not call docker-compose up!"))
		return
	}
}

func dockerComposeDown(writer http.ResponseWriter, reqest *http.Request) {

	// Assumes the docker-compose.yml is in its /work directory
	// Run in detach mode.
	cmdArgs := []string{"-f", "/work/docker-compose.yml", "down"}
	_, err := exec.Command("docker-compose", cmdArgs...).Output()

	// Catch all errors.
	if err != nil {
		writer.WriteHeader(http.StatusUnprocessableEntity)
		writer.Write([]byte("Could not call docker-compose down!"))
		return
	}
}
