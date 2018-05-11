/**
 * Basic DNS server for the IncludeOS unikernel framework.
 *
 * This code uses portions of code created by GitHub user tomasorti for his dns-server project (https://github.com/tomasorti/dns-server)
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <map>
#include <vector>

#define SERVER_PORT 53
#define BUFFER_SIZE 1024

static const unsigned int QR_MASK = 0x8000;
static const unsigned int OPCODE_MASK = 0x7800;
static const unsigned int AA_MASK = 0x0400;
static const unsigned int TC_MASK = 0x0200;
static const unsigned int RD_MASK = 0x0100;
static const unsigned int RA_MASK = 0x8000;
static const unsigned int RCODE_MASK = 0x000F;
static const unsigned int HDR_OFFSET = 12;

// for more info on DNS fields, see RFC 1035

struct queryHeader {
	unsigned int id;			// 16 bit identifier assigned by the program who generated the query
	unsigned int query;			// identifies if message is query (0) or a response (1)
	unsigned int opcode;		// 4 bit field specifying kind of query: standard (0), inverse (1), server status request (2), reserved (3-15)
	unsigned int aa;			// bit indicating if responding server is an authority for the domain name
	unsigned int truncation;			// specifies whether this message was truncated or not
	unsigned int recursionDesired;		// may be set in a query, directs the name server to pursue the query recursively
	unsigned int recursionAvailable;	// set in a response, indicates whether recursive queries are available
	unsigned int responseCode;			// 4 bit response code: 0 - no error, 1 - format error, 2 - server failure, 3 - name error, 4 - not implemented, 5 - refused

	// unsigned 16-bit integers
	unsigned int questionCount;				// number of entries in the question section
	unsigned int answerCount;				// number of resource reconds in the answer section
	unsigned int nameServerCount;			// number of name server resource records in the authority records section
	unsigned int additionalResourceCount;	// number of resource records in the additional records section
};

struct queryQuestion {
	std::string queryName;	// domain name requested
    unsigned int queryType;			// 2 octet code specifying the type of the query
    unsigned int queryClass;		// 2 octet code specifying the class of the query
};

queryHeader pQueryHeader;
queryQuestion pQueryQuestion;
std::map<std::string,std::vector<int>> resourceRecords;

unsigned int get16bits(char*& buffer) {
	unsigned int value = static_cast<unsigned char> (buffer[0]);
    value = value << 8;
    value += static_cast<unsigned char> (buffer[1]);
    buffer += 2;
	
    return value;
}

void put8bits(char*& buffer, unsigned int value) throw () {
    buffer[0] = (value);
    buffer += 1;
}

void put16bits(char*& buffer, unsigned int value) throw () {
    buffer[0] = (value & 0xFF00) >> 8;
    buffer[1] = value & 0xFF;
    buffer += 2;
}

void put32bits(char*& buffer, unsigned long value) throw () {
    buffer[0] = (value & 0xFF000000) >> 24;
    buffer[1] = (value & 0xFF0000) >> 16;
    buffer[2] = (value & 0xFF00) >> 16;
    buffer[3] = (value & 0xFF) >> 16;
    buffer += 4;
}

queryHeader decodeHeader(char* data, queryHeader pQueryHeader){
	pQueryHeader.id = get16bits(data);

    unsigned int fields = get16bits(data);
    pQueryHeader.query = fields & QR_MASK;
    pQueryHeader.opcode = fields & OPCODE_MASK;
    pQueryHeader.aa = fields & AA_MASK;
    pQueryHeader.truncation = fields & TC_MASK;
    pQueryHeader.recursionDesired = fields & RD_MASK;
    pQueryHeader.recursionAvailable = fields & RA_MASK;

    pQueryHeader.questionCount = get16bits(data);
    pQueryHeader.answerCount = get16bits(data);
    pQueryHeader.nameServerCount = get16bits(data);
    pQueryHeader.additionalResourceCount = get16bits(data);
	return pQueryHeader;
}

queryQuestion decodeQuery(char*& data, queryQuestion pQueryQuestion){
	pQueryQuestion.queryName = "";

    int length = *data++;
    while (length != 0) {
        for (int i = 0; i < length; i++) {
            char c = *data++;
            pQueryQuestion.queryName.append(1, c);
        }
        length = *data++;
        if (length != 0) pQueryQuestion.queryName.append(1,'.');
    }
	
	pQueryQuestion.queryType = get16bits(data);
    pQueryQuestion.queryClass = get16bits(data);
	return pQueryQuestion;
}

void codeDomain(char*& buffer, const std::string& domain) {
	int start = 0, end; // indexes
    while ((end = domain.find('.', start)) != std::string::npos) {
        *buffer++ = end - start; // label length octet
        for (int i=start; i<end; i++) {
            *buffer++ = domain[i]; // label octets
        }
        start = end + 1; // Skip '.'
    }

    *buffer++ = domain.size() - start; // last label length octet
    for (int i=start; i<domain.size(); i++) {
        *buffer++ = domain[i]; // last label octets
    }
    *buffer++ = 0;
}


int processQuery(char* buffer, queryHeader pQueryHeader, queryQuestion pQueryQuestion){
	
	// search for domain in resourceRecords
	bool bRRexists = true;
	std::map<std::string,std::vector<int>>::iterator record = resourceRecords.find(pQueryQuestion.queryName);
	if (record == resourceRecords.end())
		bRRexists = false;		
	
	char* bufferBegin = buffer;
	 
	// create header
	put16bits(buffer, pQueryHeader.id);

    int fields = (1 << 15);		// Response code
    fields += (0 << 11);		// Opcode	
    fields += (0 << 10);		// Authoritative code
	fields += (0 << 9);			// Truncated code
	fields += (1 << 8);			// Recursion desired code
	fields += (0 << 7);			// Recursion available code
	fields += (0 << 6);			// Z reserved code
	fields += (0 << 5);			// Answers authenticated code
	fields += (0 << 4);			// Non-authenticated data code
	if(bRRexists && pQueryQuestion.queryType == 1)
		fields += 0;			// Reply code
	else
		fields += 3;
    put16bits(buffer, fields);

    put16bits(buffer, pQueryHeader.questionCount);		// Questions count
	if(bRRexists && pQueryQuestion.queryType == 1)
		put16bits(buffer, 1);	// Answers count
	else
		put16bits(buffer, 0);
    put16bits(buffer, 0);	// Authority RRs
    put16bits(buffer, 0);	// Additional RRs
	
	// create domain query section - copy original question format
	codeDomain(buffer, pQueryQuestion.queryName);
	put16bits(buffer, pQueryQuestion.queryType);
    put16bits(buffer, pQueryQuestion.queryClass);
	
	// if requested domain exists in resourceRecords, create the answer
	if(bRRexists && pQueryQuestion.queryType == 1){
		// compression code to pointing to original question - should be offset from ID to domain name (in number of 16bits)
		put16bits(buffer, 49164);
		
		put16bits(buffer, pQueryQuestion.queryType);
		put16bits(buffer, pQueryQuestion.queryClass);
		put32bits(buffer, 0);		// time to live
		
		put16bits(buffer, 4);		// data length (always 4 for IPv4 addresses)
		put8bits(buffer,resourceRecords[pQueryQuestion.queryName][0]);
		put8bits(buffer,resourceRecords[pQueryQuestion.queryName][1]);
		put8bits(buffer,resourceRecords[pQueryQuestion.queryName][2]);
		put8bits(buffer,resourceRecords[pQueryQuestion.queryName][3]);
	}
	
    int size = buffer - bufferBegin;
	
	return size;
}

int main(int argc, char *argv[])
{
	resourceRecords["www.cetic.be"].push_back(10);
	resourceRecords["www.cetic.be"].push_back(0);
	resourceRecords["www.cetic.be"].push_back(0);
	resourceRecords["www.cetic.be"].push_back(5);
	resourceRecords["cetic.be"].push_back(10);
	resourceRecords["cetic.be"].push_back(0);
	resourceRecords["cetic.be"].push_back(0);
	resourceRecords["cetic.be"].push_back(5);
	resourceRecords["internship.cetic.be"].push_back(10);
	resourceRecords["internship.cetic.be"].push_back(0);
	resourceRecords["internship.cetic.be"].push_back(0);
	resourceRecords["internship.cetic.be"].push_back(5);
	
	// define address and socket variables
	struct sockaddr_in serverAddress;
	int serverSocket;
    struct sockaddr_in clienAddress;
    socklen_t clientAddressLength = sizeof(clienAddress);
    int bytesReceivedLength;
    char receiveBuffer[BUFFER_SIZE];
	
	// creating UDP socket
	if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket");
		return 0;
	}
	
	// defining server address properties
	memset((void *)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
	//serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(SERVER_PORT);
	
	// binding to UDP socket
	if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
		perror("bind failed");
		return 0;
	}
	
	printf("Service IP address is %s on port %d\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
	
	for (;;) {
		bytesReceivedLength = recvfrom(serverSocket, receiveBuffer, BUFFER_SIZE, 0, (struct sockaddr *)&clienAddress, &clientAddressLength);
		printf("received %d bytes\n", bytesReceivedLength);
		if (bytesReceivedLength > 0) {
			char* data = &receiveBuffer[0];
			
			pQueryHeader = decodeHeader(data, pQueryHeader);
			data += HDR_OFFSET;
			pQueryQuestion = decodeQuery(data, pQueryQuestion);
			
			// debug output
			printf("####### INCOMMING REQUEST #######\n");
			printf("Getting UDP data from %s:%d\n", inet_ntoa(clienAddress.sin_addr), ntohs(clienAddress.sin_port));
			printf("Header:\n");
			printf("\tID: %u\n",pQueryHeader.id);
			printf("\tQuery: %u\n", pQueryHeader.query);
			printf("\topCode: %u\n", pQueryHeader.opcode);
			printf("\tAA: %u\n", pQueryHeader.aa);
			printf("\tTruncation: %u\n", pQueryHeader.truncation);
			printf("\tRecursion Desired: %u\n", pQueryHeader.recursionDesired);
			printf("\tRecursion Available: %u\n", pQueryHeader.recursionAvailable);
			printf("\tReponse Code: %u\n", pQueryHeader.responseCode);
			printf("\tQuestion Count: %u\n", pQueryHeader.questionCount);
			printf("\tAnswer Count: %u\n", pQueryHeader.answerCount);
			printf("\tName Server Count: %u\n", pQueryHeader.nameServerCount);
			printf("\tAdditional Resource Count: %u\n", pQueryHeader.additionalResourceCount);
			printf("Query:\n");
			printf("\tQuery name: %s\n", pQueryQuestion.queryName.data());
			printf("\tQuery Type: %u\n", pQueryQuestion.queryType);
			printf("\tAdditional Resource Count: %u\n", pQueryQuestion.queryClass);
			printf("#################################\n"); 
			
			// send reply
			char buffer[BUFFER_SIZE];
			int nbrBytes = processQuery(buffer, pQueryHeader, pQueryQuestion);
			
			sendto(serverSocket, buffer, nbrBytes, 0, (struct sockaddr *)&clienAddress, clientAddressLength);
		}
	}
}
