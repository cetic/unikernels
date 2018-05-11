# Container Code

This folder contains the container version of the unikernel services.

Each folder holds:
* The CPP file containing the code
* A **compile.sh** script to compile the CPP file into a runnable service
* A **Dockerfile** to create the container for this service
* A **create_container.sh** script to create to container from the Dockerfile
* A **run_service.sh** script to run the created container in detached mode
* A **start.sh** script that will compile the binary from code, build the Docker container and run it (in detached mode)

The following section briefly explains the steps taken.

## Pre Requisite

Before running the **compile.sh** or **start.sh** scripts, you must first build the **builder** container from the Dockerfile in this parent folder.

This can be done by running the **create_compiler.sh** script or running the following command from the CONTAINER directory:

```
docker build -t cetic/compiler .
```

## Compiling the binary (*compile.sh*)

*Requires the cetic/compiler container image*

This script will launch a new cetic/compiler image container, mapping the current folder to a folder inside the container, then it will staticaly compile the C++ code in the .cpp file in the directory and output a *runnableService* binary.

## Creating the container

This will create the service container using the **Dockerfile**.

It will use the lastest version of alpine base image and copy the **runnableService** executable inside the container.

## Running the container

When running the container, it will execute the command instructed in the **Dockerfile**.

The container will wait for the network to be up (sleep 1) then it will launch the service binary, passing the container's IP address to bind the service to as a paramater.

The script starts it in detached mode (i.e.: in the background)

**NB:** as of now, the IP address is not given statically, you must run `docker network inspect bridge` to view the IP address assigned to the container.