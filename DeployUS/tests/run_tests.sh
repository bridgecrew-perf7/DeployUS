#!/usr/bin/env bash
# Script heavily inspired from Bohumír Zámečník and his tutorial at https://bohumirzamecnik.cz/blog/2018/gitlab-docker-compose-tests/

# Print commands
set -x

# Shortcut the bash command
docker_compose_cmd="docker-compose -f docker-compose.tests.yml"

# Run service containers, except for test container
${docker_compose_cmd} up -d --no-deps db app

# Run the tests
${docker_compose_cmd} run --rm tests

# keep the exit code from tests while we clean up the containers
exit_code=$?

# Clean up
${docker_compose_cmd} down

# return the original result of the test - for GitLab CI
exit ${exit_code}