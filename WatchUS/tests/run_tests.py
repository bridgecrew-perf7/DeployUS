#!/usr/bin/python3
"""
WatchUS/tests/run_tests.py

Cross-Platform script that executes WatchUS tests.
To run, it may be necessary to have administrator priviledges (or sudo in unix)

External dependencies:
    - Must have docker and docker-compose installed on the host system

Acknowledgements
1.  Script heavily inspired from Bohumír Zámečník
    and his tutorial at https://bohumirzamecnik.cz/blog/2018/gitlab-docker-compose-tests/
"""
import os
import pathlib
import sys

# Navigation to the right directiory (WatchUS)
WatchUS_dir = pathlib.Path(__file__).absolute().parent.parent

# Shortcut the bash command
docker_compose_cmd = (
    f"cd {WatchUS_dir};" + " docker-compose -f docker-compose.tests.yml"
)

# Build the tests image form its docker file. The other images will be pulled from docker hub
cmd = docker_compose_cmd + " build tests "
os.system(cmd)

# Run service containers, except for test container, as we need its exit code for gitlab ci
cmd = docker_compose_cmd + " pull "
os.system(cmd)
cmd = docker_compose_cmd + " up -d --no-deps app "
os.system(cmd)

# Run the tests and keep the exit_code (useful for gitlab ci)
cmd = docker_compose_cmd + " run --rm tests "
exit_code = os.system(cmd)

# Clean up. simple docker-compose down
cmd = docker_compose_cmd + " down "
os.system(cmd)

# Return the results of the tests
sys.exit(exit_code)
