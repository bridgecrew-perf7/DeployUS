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
		panic(errJSON)
	}
	defer reqest.Body.Close()

	// Write the file to disk at /work/docker-compose.yml
	file, err := os.Create("/work/docker-compose.yml")
	file.WriteString(toWatch.File)
	if err != nil {
		fmt.Println("Creating docker-compose.yml file does not work!", err)
	}

	// With the docker-compose.yml file in the /work directory, we can now launch it!
	// Pull necessary images
	cmdArgs := []string{"-f", "/work/docker-compose.yml", "pull"}
	_, err = exec.Command("docker-compose", cmdArgs...).Output()
	if err != nil {
		fmt.Println("docker-compose does not work!", err)
	}

	// Run in detach mode.
	cmdArgs = []string{"-f", "/work/docker-compose.yml", "up", "-d", "--force-recreate"}
	_, err = exec.Command("docker-compose", cmdArgs...).Output()

	// Catch all errors, useful for CI
	if err != nil {
		fmt.Printf("%s", err)
	}
}

func dockerComposeDown(writer http.ResponseWriter, reqest *http.Request) {

	// Assumes the docker-compose.yml is in its /work directory
	// Run in detach mode.
	cmdArgs := []string{"-f", "/work/docker-compose.yml", "down"}
	_, err := exec.Command("docker-compose", cmdArgs...).Output()

	// Catch all errors.
	if err != nil {
		fmt.Printf("%s", err)
	}
}
