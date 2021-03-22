#!/usr/bin/python3
# Script heavily inspired from Bohumír Zámečník and his tutorial at https://bohumirzamecnik.cz/blog/2018/gitlab-docker-compose-tests/
import os
import sys

# Shortcut the bash command
docker_compose_cmd = "docker-compose -f docker-compose.tests.yml"

# Build the tests image form its docker file. The other images will be pulled from docker hub
cmd = docker_compose_cmd + " build tests "
os.system(cmd)

# Run service containers, except for test container, as we need its exit code for gitlab ci
cmd = docker_compose_cmd + " pull "
os.system(cmd)
cmd = docker_compose_cmd + " up -d --no-deps --no-build db app "
os.system(cmd)

# Run the tests
# keep the exit code
cmd = docker_compose_cmd + " run --rm tests "
exit_code = os.system(cmd)

# Clean up. simple docker-compose down
cmd = docker_compose_cmd + " down "
os.system(cmd)

# Return the results of the tests
sys.exit(exit_code)