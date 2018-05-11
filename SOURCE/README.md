# Source Code

This folder contains the source code of the unikernel projects created with the [IncludeOS framework](https://github.com/hioa-cs/IncludeOS)

Each folder contains the source code for a single unikernel.

## Building Unikernel Image

**Requires IncludeOS to be installed on the system**

The script **cmake_build.sh** can be executed to build the each unikernel images. The .img disk file will be located in the **build** folder inside the project folder.

## Container Versions

The **CONTAINER** folder contains the container versions of the unikernel images.

The container versions are aimed to be analogs of the unikernel code but adapted to run in a standalone Docker container.

## Deployment

For deploying the unikernel or container environment, see the [DEPLOYMENT](https://git.cetic.be/stages/unikernels/tree/feature/stagelongree2018/DEPLOYMENT) folder for deployment scripts.