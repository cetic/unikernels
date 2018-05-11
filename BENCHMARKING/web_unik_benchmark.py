#!/usr/bin/python3
import sys
import subprocess
import shutil
import os.path
import csv
import time
import string

RUN=1
TIME=300
CONNECTIONS=100
THREADS=4
QPS=100
QPS_INCREMENT=100

if __name__ == '__main__':
	if len(sys.argv) < 2:
		print("DNS Server Benchmarking Tool - CETIC Internship 2017/18 on Unikernels\n\tUsage: python3 web_unik_benchmark.py [server_ip_address]")
		sys.exit(1)
	
	SVR_IP=sys.argv[1]
	
	if shutil.which("dnsperf") is None:
		print("ERROR: dnsperf is not installed, please run the install_bench_tools.sh script.")
		sys.exit(1)
		
	if os.path.isfile("web_unik_benchmark.csv") is not True:
		subprocess.run("touch web_unik_benchmark.csv", stdout=subprocess.PIPE, shell=True)
	
	with open("web_unik_benchmark.csv", "w") as file:
		writer = csv.writer(file)
		writer.writerow(["TEST_RUN", "QUERIES_SENT", "QUERIES_PER_SECOND", "AVG_LATENCY_MILLISECONDS", "MAX_LATENCY_MILLISECONDS", "LATENCY_STD_DEV_PERCENT"])
		
	while True:
		print("########## LAUNCHING TEST " + str(RUN) + " ##########")
		print("Queries per seconds: " + str(QPS))
		# launch the dnsperf command
		result = subprocess.run("wrk2 -t " + str(THREADS) + " -d " + str(TIME) + "s -c " + str(CONNECTIONS) + " -R " + str(QPS) + " http://" + SVR_IP + "/", stdout=subprocess.PIPE, shell=True)
		#print(result.stdout.decode('utf-8'))
		
		# convert output and store it in the csv file
		list = result.stdout.decode("utf-8").split('\n')
		for index in range(len(list)):
			if list[index].startswith("    Latency"):
				avgLatency=list[index].split()[1].strip(string.ascii_letters)
				latencyMax=list[index].split()[3].strip(string.ascii_letters)
				latencyStdDev=list[index].split()[2].strip(string.ascii_letters)
				queriesSent=list[index+2].split()[0]
				queriesPerSecond=list[index+3].split()[1]
				
				print("Queries sent:\t\t" + queriesSent + "\nQueries per second:\t" + queriesPerSecond + "\nAverage latency:\t" + avgLatency + "ms" + \
						"\nMax latency:\t\t" + latencyMax + "ms" + "\nLatency Std Dev:\t" + latencyStdDev + "ms")
				with open("web_unik_benchmark.csv", "a") as file:
					writer = csv.writer(file)
					writer.writerow([RUN, queriesSent, queriesPerSecond, avgLatency, latencyMax, latencyStdDev])
				break
		
		print("########## TEST " + str(RUN) + " COMPLETE ##########\n")
		# increase the TEST_RUN value and increment the number of queries per second
		RUN=RUN+1
		QPS=QPS+QPS_INCREMENT
		# sleep for 30 seconds between tests
		time.sleep(30)