package tests

import (
	"fmt"
	"strings"
	"testing"
	"os"
	"os/exec"
	"io/ioutil"
	"net/http"
	"github.com/go-resty/resty/v2"
	"github.com/stretchr/testify/assert"
)

func TestMain(m *testing.M) {
    setup()
    code := m.Run() 
    os.Exit(code)
}

func setup() {
	// Copying the dummy docker-compose file to the /work directory
	cmdArgs := []string{"./docker-compose.dummy.yml", "/work/docker-compose.yml"}
	exec.Command("cp", cmdArgs...).Output()
}

func build_dummyexample(text string) {
	docker_file_format := `
FROM httpd:latest
RUN echo -n "%s" > /usr/local/apache2/htdocs/index.html
WORKDIR /usr/local/bin/
CMD ["httpd-foreground"]
	`

	// Writing the docker file to file
	file, _:= os.Create("/work/Dockerfile.dummy")
	file.WriteString(fmt.Sprintf(docker_file_format, text))

	// Building the docker image
	cmdArgs := []string{"build", "-t", "dummyexample", "-f", "/work/Dockerfile.dummy", "/work"}
	exec.Command("docker", cmdArgs...).Output()


}

func start_dummy_docker_compose(t *testing.T, assert *assert.Assertions, client *resty.Client ) int {
	// To start, save the number of running containers. 
	// Later, we will be starting a new container and verifying that 
	// the docker-compose cammand has worked. 
	cmdArgs := []string{"ps", "-q"}
	out, err := exec.Command("docker", cmdArgs...).Output()
	assert.Nil(err)
	base_num_containers := strings.Count(string(out[:]), "\n")

	// We send a request to WatchUs to start the container
	resp, _ := client.R().Get("http://app:5001/up")
	if resp.StatusCode() != 200 {
		t.Errorf("Unexpected status code, expected %d, got %d instead", 200, resp.StatusCode())
	}

	// We count the number of containers after running WatchUS went up
	cmdArgs = []string{"ps", "-q"}
	out, err = exec.Command("docker", cmdArgs...).Output()
	assert.Nil(err)
	new_num_containers := strings.Count(string(out[:]), "\n")

	// Assert that the number of containers hasn't at least decreased!
	assert.True(new_num_containers >= base_num_containers)

	return base_num_containers
}

func stop_dummy_docker_compose(t *testing.T, assert *assert.Assertions, client *resty.Client, base_num_containers int) {
	
	// Kill the app created as to not affect the other tests
	resp, _ := client.R().Get("http://app:5001/down")
	if resp.StatusCode() != 200 {
		t.Errorf("Unexpected status code, expected %d, got %d instead", 200, resp.StatusCode())
	}

	// We count the number of containers after running WatchUS went up
	cmdArgs := []string{"ps", "-q"}
	out, err := exec.Command("docker", cmdArgs...).Output()
	assert.Nil(err)
	new_num_containers := strings.Count(string(out[:]), "\n")
	assert.True(new_num_containers == base_num_containers)
}

/*
	Actual Test Functions
*/

func Test_docker_compose_up_then_down(t *testing.T) {
	assert := assert.New(t)
	client := resty.New()
	
	// Start a dummy application
	index_file_contents := "thecontents"
	build_dummyexample(index_file_contents)
	base_num_containers := start_dummy_docker_compose(t, assert, client)

	// Send request to the dummyexample page
	resp, _ := http.Get("http://dummy/")
	defer resp.Body.Close()

	// Read the html portion, which contains the index file contents
	html, _ := ioutil.ReadAll(resp.Body)

	// show the HTML code as a string %s
	assert.Equal(index_file_contents, string(html[:]) )

	// Close the dummy application. Must do this for other tests
	stop_dummy_docker_compose(t, assert, client, base_num_containers)
}


func Test_docker_compose_up_reload_image(t *testing.T) {
	assert := assert.New(t)
	client := resty.New()

	// Start a dummy application
	index_file_contents := "thecontents"
	build_dummyexample(index_file_contents)
	base_num_containers := start_dummy_docker_compose(t, assert, client)

	// Read the html portion, which contains the index file contents
	// The html contents needs to be what we sent!
	resp, _ := http.Get("http://dummy/")
	defer resp.Body.Close()
	html, _ := ioutil.ReadAll(resp.Body)
	assert.Equal(index_file_contents, string(html[:]) )


	// Change the file contents and do a docker-compose up again!
	// Note that the running container wasn't stopped prior to this.
	index_file_contents = "The contents changed!"
	build_dummyexample(index_file_contents)
    start_dummy_docker_compose(t, assert, client)

	resp, _ = http.Get("http://dummy/")
	defer resp.Body.Close()
	html, _ = ioutil.ReadAll(resp.Body)
	assert.Equal(index_file_contents, string(html[:]) )

	// Close the dummy application. Must do this for other tests
	stop_dummy_docker_compose(t, assert, client, base_num_containers)
}

