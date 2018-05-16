# Deployment

This folder contains the files to deploy the proof of concept environment.

## pre_deployment_installation.sh

Installs all the required components to launch both the unikernel and the container environment.

Install includes:
* IncludeOS project installation
* libvirt for unikernel virtual machines deployment
* Docker CE for container deployments

**NOTE:** installation of the Docker CE adds the current user to the docker group, you must logout/login for the changes to take effect.

## deploy_unikernels.sh

This script will deploy the unikernels by taking the following steps:
* building the unikernels from the source code (in the SOURCE directory)
* create the QEMU networks for the unikernels virtual machine
* create and launch the virtual machines in their respective networks

The networks are created with the following addressing:

![Proof of concept network addressing scheme](https://raw.githubusercontent.com/cetic/unikernels/master/MEDIA/unikernel_network.PNG "Proof of concept network addressing scheme")

| VM          | IP Address      | 
| ----------- |-----------------|
| Firewall    | 192.168.100.254 |
| Firewall    | 192.168.101.3   |
| Router      | 192.168.101.2   |
| Router      | 10.0.0.254      |
| Web server  | 10.0.0.5        |
| DNS server  | 10.0.0.100      |

The current proof of concept environment and is self contained. It is not possible to directly connect the firewall to the external network.
It is possible to contact the DNS and the web server through iptables rules (examples in the benchmark launching scripts).

## deploy_containers.sh

This script will deploy the containers (using ported code from the unikernel versions) by taking the following steps:
* building the compiler image
* running the compiler image to cross-compile the application
* build the application container using the compiled application program
* run the container images built in the previous step

The container ports are mapped as follows:

| Container   | Host Port  | Guest Port | 
| ----------- | ---------- | ---------- |
| Web server  | 80/tcp     | 80/tcp     |
| DNS server  | 53/udp     | 53/udp     |

**NOTE:** due to time constraints during the project, the firewall and router applications were not ported to containers.
