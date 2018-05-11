#!/usr/bin/python3
import sys
import subprocess
import os.path
import csv
import json
import time
import _thread
import os
import datetime as datetime
import random

BENCHMARK_TIME_SECONDS=60*20
MAX_NBR_VMS=150
NBR_VMS_INCREMENT=10
GUESTS=dict()
NBR_VMS=0

def run_cmd(COMMAND):
	return subprocess.run(COMMAND, stdout=subprocess.PIPE, shell=True)

def timeGuestStart(GUEST):
	response = 1
	startTime=datetime.datetime.now()
	while response is not 0:
		response = os.system("ping -c 1 " + GUESTS[GUEST] + "> /dev/null 2>&1")
	endTime=datetime.datetime.now()
	bootTime=(endTime-startTime).microseconds
	with open("startup_cont_benchmark.csv", "a") as file:
		writer = csv.writer(file)
		writer.writerow([NBR_VMS, GUEST, (bootTime/1000000)])	

if __name__ == '__main__':
	if len(sys.argv) < 2:
		print("Application Startup Benchmarking Tool - CETIC Internship 2017/18 on Unikernels\n\tUsage: python3 startup_cont_benchmark.py [nbr of VMs]")
		sys.exit(1)
	
	NBR_VMS=int(sys.argv[1])
		
	if os.path.isfile("startup_cont_benchmark.csv") is not True:
		subprocess.run("touch startup_cont_benchmark.csv", stdout=subprocess.PIPE, shell=True)
		with open("startup_cont_benchmark.csv", "w") as file:
			writer = csv.writer(file)
			writer.writerow(["NBR_OF_GUESTS", "GUEST_STOPPED", "STARTUP_TIME_SECONDS"])
	
	while NBR_VMS < MAX_NBR_VMS:
		print("### STOPPING PREVIOUS VMS")
		# stop all running VMs
		run_cmd("docker stop $(docker ps -a -q) 2> /dev/null")
		run_cmd("virsh list | grep running | awk '{ print $2}' | while read DOMAIN; do virsh destroy $DOMAIN; sleep 3; virsh undefine $DOMAIN; done")
		run_cmd("docker ps -a -q | while read DOMAIN; do docker rm $DOMAIN; done")
		
		# create table of guest VMs
		GUESTS.clear()
		i = 10
		for x in range(int(NBR_VMS)):
			GUESTS[("VM_"+str(x))] = ("172.18.0." + str(i))
			i = i+1
			
		print("### BUILDING CONTAINERS")
		run_cmd("cd ../SOURCE/CONTAINER && docker build -t cetic/compiler .")
		run_cmd("cd ../SOURCE/CONTAINER/DNS &&\
					docker run --rm -it -v $PWD:/usr/src/build -w /usr/src/build cetic/compiler g++ -std=c++11 -static -o runnableService *.cpp &&\
					docker build -t cetic/dns .")
		run_cmd("docker network create --subnet=172.18.0.0/16 --driver=bridge bench_net 2> /dev/null")
		
		print("### LAUNCHING CONTAINERS")
		# create and launch each VM
		for key, value in GUESTS.items():
			# for each entry in GUESTS build the unikernel and move the img to another folder
			
			run_cmd("docker run --cpus 1 -m 128m -d --ip " + value + " --network=bench_net --name " + key + " cetic/dns 2> /dev/null")
						
		print("### LAUNCHING BENCHMARK FOR " + str(BENCHMARK_TIME_SECONDS) + " SECONDS")
		
		timeout=time.time() + BENCHMARK_TIME_SECONDS 
		while time.time() <= timeout:
			# stop a VM
			RANDOM_GUEST="VM_"+str(random.randint(0, (int(NBR_VMS)-1)))
			run_cmd("docker stop " + RANDOM_GUEST)
			print(RANDOM_GUEST + " shut down")
			# launch a thread to start a VM
			_thread.start_new_thread(run_cmd, ("docker start " + RANDOM_GUEST, ))
			# launch another thread to time boot time
			_thread.start_new_thread(timeGuestStart, (RANDOM_GUEST, ))
			print(RANDOM_GUEST + " started...")
			time.sleep(5)
			
		#increment number of VMs for next test
		NBR_VMS=NBR_VMS+NBR_VMS_INCREMENT
		
	run_cmd("docker stop $(docker ps -a -q) 2> /dev/null")
	
	