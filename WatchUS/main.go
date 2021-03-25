package main

import (
	"fmt"
	"net/http"
	"os/exec"
)

func main() {
	http.HandleFunc("/up", dockerComposeUp)
	http.HandleFunc("/down", dockerComposeDown)

	// DeployUS is on port 5000, WatchUS will be on port 5001
	http.ListenAndServe(":5001", nil)
}

func dockerComposeUp(writer http.ResponseWriter, reqest *http.Request) {
	
	// Assumes the docker-compose.yml is in its /work directory
	// Run in detach mode. 
	cmdArgs := []string{"-f", "/work/docker-compose.yml", "up", "-d", "--force-recreate"}
	out, err := exec.Command("docker-compose", cmdArgs...).Output()

	// Catch all errors, useful for CI
	if err != nil {
        fmt.Printf("%s", err)
    }

	// Priting command output for dugging purposes.
	fmt.Println("Command UP Successfully Executed")
    output := string(out[:])
    fmt.Println(output)
}

func dockerComposeDown(writer http.ResponseWriter, reqest *http.Request) {
	
	// Assumes the docker-compose.yml is in its /work directory
	// Run in detach mode. 
	cmdArgs := []string{"-f", "/work/docker-compose.yml", "down"}
	out, err := exec.Command("docker-compose", cmdArgs...).Output()

	// Catch all errors.
	if err != nil {
        fmt.Printf("%s", err)
    }

	fmt.Println("Command DOWN Successfully Executed")
    output := string(out[:])
    fmt.Println(output)
}

