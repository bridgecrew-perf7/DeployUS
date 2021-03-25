package tests

import (
	"strings"
	"testing"
	"os"
	"os/exec"
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
	exec.Command("cp", cmdArgs...)
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

	// Assert that at least one new container was started
	assert.True(new_num_containers > base_num_containers)

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

func Test_docker_compose_up_then_down(t *testing.T) {
	assert := assert.New(t)
	client := resty.New()

	// Start a dummy application
	base_num_containers := start_dummy_docker_compose(t, assert, client)

	// Close the dummy application
	stop_dummy_docker_compose(t, assert, client, base_num_containers)
}
