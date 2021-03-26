package tests

import (
	"fmt"
	"github.com/go-resty/resty/v2"
	"github.com/stretchr/testify/assert"
	"io/ioutil"
	"net/http"
	"os"
	"os/exec"
	"strings"
	"testing"
)

func TestMain(m *testing.M) {
	// Running setup code before executing tests
	setup()
	code := m.Run()
	os.Exit(code)
}

func setup() {
	// Copying the dummy docker-compose file to the /work directory
	cmdArgs := []string{"./docker-compose.dummy.yml", "/work/docker-compose.yml"}
	exec.Command("cp", cmdArgs...).Output()
}

func buildDummyExample(text string) {
	// Our test image will be a simple httpd webserver
	dockerFileFormat := `
FROM httpd:latest
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

func startDockerDummyCompose(t *testing.T, assert *assert.Assertions, client *resty.Client) int {
	// To start, save the number of running containers.
	// Later, we will be starting a new container and verifying that
	// the docker-compose cammand has worked.
	cmdArgs := []string{"ps", "-q"}
	out, err := exec.Command("docker", cmdArgs...).Output()
	assert.Nil(err)
	baseNumContainers := strings.Count(string(out[:]), "\n")

	// We send a request to WatchUs to start the container
	resp, _ := client.R().Get("http://app:5001/up")
	if resp.StatusCode() != 200 {
		t.Errorf("Unexpected status code, expected %d, got %d instead", 200, resp.StatusCode())
	}

	// We count the number of containers after running WatchUS went up
	cmdArgs = []string{"ps", "-q"}
	out, err = exec.Command("docker", cmdArgs...).Output()
	assert.Nil(err)
	newNumContainers := strings.Count(string(out[:]), "\n")

	// Assert that the number of containers hasn't at least decreased!
	assert.True(newNumContainers >= baseNumContainers)

	return baseNumContainers
}

func stopDummyDockerCompose(t *testing.T, assert *assert.Assertions, client *resty.Client, baseNumContainers int) {

	// Kill the app created as to not affect the other tests
	resp, _ := client.R().Get("http://app:5001/down")
	if resp.StatusCode() != 200 {
		t.Errorf("Unexpected status code, expected %d, got %d instead", 200, resp.StatusCode())
	}

	// We count the number of containers after running WatchUS went up
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
	client := resty.New()

	// Start a dummy application
	indexFileContents := "thecontents"
	buildDummyExample(indexFileContents)
	baseNumContainers := startDockerDummyCompose(t, assert, client)

	// Send request to the dummyexample page
	resp, _ := http.Get("http://dummy/")
	defer resp.Body.Close()

	// Read the html portion, which contains the index file contents
	html, _ := ioutil.ReadAll(resp.Body)

	// show the HTML code as a string %s
	assert.Equal(indexFileContents, string(html[:]))

	// Close the dummy application. Must do this for other tests
	stopDummyDockerCompose(t, assert, client, baseNumContainers)
}

func Test_docker_compose_up_reload_image(t *testing.T) {
	assert := assert.New(t)
	client := resty.New()

	// Start a dummy application
	indexFileContents := "thecontents"
	buildDummyExample(indexFileContents)
	baseNumContainers := startDockerDummyCompose(t, assert, client)

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
	startDockerDummyCompose(t, assert, client)

	resp, _ = http.Get("http://dummy/")
	defer resp.Body.Close()
	html, _ = ioutil.ReadAll(resp.Body)
	assert.Equal(indexFileContents, string(html[:]))

	// Close the dummy application. Must do this for other tests
	stopDummyDockerCompose(t, assert, client, baseNumContainers)
}
