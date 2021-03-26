package tests

import (
	"bytes"
	"encoding/json"
	"fmt"
	"github.com/stretchr/testify/assert"
	"io/ioutil"
	"net/http"
	"os"
	"os/exec"
	"strings"
	"testing"
)

func getDummyDockerCompose() (string, string) {
	// Create a dummy docker-compose.yml file
	// Returns the file contents and a project name
	return `version: "3"
services:
  dummy:
    image: dummyexample:latest
    networks:
        - net
    
networks:
  net:
    external: true
    name: my_net
`, "dummyexample"
}

func buildDummyExample(text string) {
	// Our test image will be a simple httpd webserver
	dockerFileFormat := `FROM httpd:latest
RUN echo -n "%s" > /usr/local/apache2/htdocs/index.html
WORKDIR /usr/local/bin/
CMD ["httpd-foreground"]
`

	// Writing the docker file to file
	file, _ := os.Create("/work/Dockerfile.dummy")
	file.WriteString(fmt.Sprintf(dockerFileFormat, text))

	// Building the docker image
	cmdArgs := []string{"build", "-t", "dummyexample", "-f", "/work/Dockerfile.dummy", "/work"}
	exec.Command("docker", cmdArgs...).Output()

}

func startDockerDummyCompose(t *testing.T, assert *assert.Assertions) int {
	// To start, save the number of running containers.
	// Later, we will be starting a new container and verifying that
	// the docker-compose cammand has worked.
	cmdArgs := []string{"ps", "-q"}
	out, err := exec.Command("docker", cmdArgs...).Output()
	assert.Nil(err)
	baseNumContainers := strings.Count(string(out[:]), "\n")

	// We send a request to WorkUS to start the container
	fileContents, projectname := getDummyDockerCompose()
	reqBody, err := json.Marshal(map[string]string{
		"file": fileContents,
		"name": projectname,
	})
	resp, _ := http.Post("http://app:5002/up",
		"application/json", bytes.NewBuffer(reqBody))
	if resp.StatusCode != 200 {
		body, _ := ioutil.ReadAll(resp.Body)
		fmt.Println(string(body[:]))
		t.Errorf("Unexpected status code, expected %d, got %d instead", 200, resp.StatusCode)
	}

	// We count the number of containers after running WorkUS went up
	cmdArgs = []string{"ps", "-q"}
	out, err = exec.Command("docker", cmdArgs...).Output()
	assert.Nil(err)
	newNumContainers := strings.Count(string(out[:]), "\n")

	// Assert that the number of containers hasn't at least decreased!
	assert.True(newNumContainers >= baseNumContainers)

	return baseNumContainers
}

func stopDummyDockerCompose(t *testing.T, assert *assert.Assertions, baseNumContainers int) {

	// Kill the app created as to not affect the other tests
	_ , projectname := getDummyDockerCompose()
	reqBody, err := json.Marshal(map[string]string{
		"name": projectname,
	})
	resp, _ := http.Post("http://app:5002/down",
		"application/json", bytes.NewBuffer(reqBody))
	if resp.StatusCode != 200 {
		body, _ := ioutil.ReadAll(resp.Body)
		fmt.Println(string(body[:]))
		t.Errorf("Unexpected status code, expected %d, got %d instead", 200, resp.StatusCode)
	}

	// We count the number of containers after running WorkUS went up
	cmdArgs := []string{"ps", "-q"}
	out, err := exec.Command("docker", cmdArgs...).Output()
	assert.Nil(err)
	newNumComntainers := strings.Count(string(out[:]), "\n")
	assert.True(newNumComntainers == baseNumContainers)
}

/*
	Actual Test Functions
*/

func Test_docker_compose_up_then_down(t *testing.T) {
	assert := assert.New(t)

	// Start a dummy application
	indexFileContents := "thecontents"
	buildDummyExample(indexFileContents)
	baseNumContainers := startDockerDummyCompose(t, assert)

	// Send request to the dummyexample page
	resp, _ := http.Get("http://dummy/")
	defer resp.Body.Close()

	// Read the html portion, which contains the index file contents
	html, _ := ioutil.ReadAll(resp.Body)

	// show the HTML code as a string %s
	assert.Equal(indexFileContents, string(html[:]))

	// Close the dummy application. Must do this for other tests
	stopDummyDockerCompose(t, assert, baseNumContainers)
}

func Test_docker_compose_up_reload_image(t *testing.T) {
	assert := assert.New(t)

	// Start a dummy application
	indexFileContents := "thecontents"
	buildDummyExample(indexFileContents)
	baseNumContainers := startDockerDummyCompose(t, assert)

	// Read the html portion, which contains the index file contents
	// The html contents needs to be what we sent!
	resp, _ := http.Get("http://dummy/")
	defer resp.Body.Close()
	html, _ := ioutil.ReadAll(resp.Body)
	assert.Equal(indexFileContents, string(html[:]))

	// Change the file contents and do a docker-compose up again!
	// Note that the running container wasn't stopped prior to this.
	indexFileContents = "The contents changed!"
	buildDummyExample(indexFileContents)
	startDockerDummyCompose(t, assert)

	resp, _ = http.Get("http://dummy/")
	defer resp.Body.Close()
	html, _ = ioutil.ReadAll(resp.Body)
	assert.Equal(indexFileContents, string(html[:]))

	// Close the dummy application. Must do this for other tests
	stopDummyDockerCompose(t, assert, baseNumContainers)
}
