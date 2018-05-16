# Benchmarking

This section provides the script to launch and reproduce the benchmarks perfromed for this project (and whoses results are posted on the root README file).

## Preparing the environment

Prior to launching the benchmarks, you must launch the [pre deployment script](https://github.com/cetic/unikernels/blob/master/DEPLOYMENT/pre_deployment_installation.sh) provided in the DEPLOYMENT folder.

Then you must install the benchmarking tools using the [install\_bench\_tools.sh](https://github.com/cetic/unikernels/blob/master/BENCHMARKING/install_bench_tools.sh) script.
This script will install the following tools:
* dnsperf and its dependencies for the DNS benchmark (by Nominum)
* wrk2 for the web server benchmark (by Gil Tene, based on original wrk by Will Glozer)

## Launching the target instance

To launch an instance to benchmark, launch the **bench_*.sh** scripts:
* bench\_container\_dns.sh
* bench\_container\_web.sh
* bench\_unikernel\_dns.sh
* bench\_unikernel\_web.sh

The scripts will:
* build the unikernel/container
* stop all running VMs and containers
* launch the unikernel/container, attaching it to the default network

If you're launching a container, the ports will be mapped on launch.
If you're launching a unikernel, two rules will be inserted in iptables to allow port forwarding to the internal network.

## Launching the benchmark

The benchmark scripts are written in python and make use of the tools installed with the install\_bench\_tools.sh script.

The web and DNS server benchmarking scripts are written to use 4 threads and simulate 100 virtual connections to the target server.

### Web server benchmark

To launch the web server, user `python3 web_unik_benchmark.py [server_ip_address]`.

The benchmark will send HTTP requests to the server for 5 minutes, starting at a pace of 100 queries per second. After 5 minutes it will save the data in a .csv file and then will relaunch the process incrementing by another 100 queries per second.

### DNS server benchmark

To launch the web server, user `python3 web_<type>_benchmark.py [server_ip_address]`.

The benchmark will send DNS requests to the server for 5 minutes, starting at a pace of 100 queries per second. After 5 minutes it will save the data in a .csv file and then will relaunch the process incrementing by another 100 queries per second.

### Boot time benchmark

To launch the web server, user `python3 startup_<type>_benchmark.py [starting number of instances]`.

The benchmark will start by building and launching the given number of VM or container instances. Then, for 20 minutes, it will send the stop a random instance, send the start command and calculate the time between the start command being sent, and when the target instance responds to a single ping. For each stop/start process, the recorded time is then inserted into a csv file. After 20 minutes, the number of instances is incremented by 10 and the process is repeated. This process repeats itself until 140 instances are launched.

## Cleaning up the environment

The individual instance scripts do not stop the instances launched, this needs to be done manually.

However, each instance start script will stop the currently running containers and virtual machines, as will the boot time benchmark scripts.

To clean up the iptable rules created for the unikernels use the `./bench_unikernel_cleanup.sh` script.

To clean up all the instances and iptable rule use the `./cleanup_all.sh` script.
