#!/usr/bin/python3
"""
run_tests.py

Cross-Platform script that executes the tests.
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
import argparse

# Parsing args
parser = argparse.ArgumentParser(description='Testing the modules for DeployUS')
parser.add_argument('-t', '--test', nargs='+',
                    default=['DeployUS','WatchUS','WorkUS'],
                    choices=['DeployUS','WatchUS','WorkUS'],
                    help="Modules to run the tests for.")
parser.add_argument('-p', '--pull', action='store_true', help = 'Pull images from docker hub instead of building. Default: building from source code.')
args = parser.parse_args()

# Navigation to the right directiory (WatchUS)
root_dir = pathlib.Path(__file__).absolute().parent

exit_code = 0;
failed_modules = []
for module in args.test:
    # Shortcut the bash command
    docker_compose_cmd = (
        f"cd {root_dir.joinpath(module)};" + " docker-compose -f docker-compose.tests.yml"
    )

    # Build the tests image form its docker file. The other images will be pulled from docker hub
    cmd = docker_compose_cmd + " build tests "
    os.system(cmd)


    if args.pull:
        # Run service containers, except for test container, as we need its exit code for gitlab ci
        cmd = docker_compose_cmd + " pull "
        os.system(cmd)
        cmd = docker_compose_cmd + " up -d app "
        os.system(cmd)
    else:
        cmd = docker_compose_cmd + " up -d --build app "
        os.system(cmd)

    # Run the tests and keep the exit_code (useful for gitlab ci)
    cmd = docker_compose_cmd + " run --rm tests "
    exit_code_cmd = os.system(cmd)
    exit_code |= exit_code_cmd
    if exit_code_cmd != 0:
        failed_modules.append(module)

    # Clean up. simple docker-compose down
    cmd = docker_compose_cmd + " down -v "
    os.system(cmd)

# Return the results of the tests
# Print module name that failed. This will make it more verbose in the CI
for module in failed_modules:
    print(f"ERROR: The {module} module's tests failed.")

# If the exit_code == 256, the uint8 unix register wont capture it.
sys.exit(1 if exit_code != 0 else 0) 
