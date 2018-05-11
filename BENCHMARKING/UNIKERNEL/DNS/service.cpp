/**
 * Basic DNS server for the IncludeOS unikernel framework.
 * 
 * This code uses portions of code created by GitHub user tomasorti for his dns-server project (https://github.com/tomasorti/dns-server)
 */

#include <service>
#include <net/inet4>
#include <map>
#include <vector>

using namespace net;

#define SERVER_PORT 53

static const uint QR_MASK = 0x8000;
static const uint OPCODE_MASK = 0x7800;
static const uint AA_MASK = 0x0400;
static const uint TC_MASK = 0x0200;
static const uint RD_MASK = 0x0100;
static const uint RA_MASK = 0x8000;
static const uint RCODE_MASK = 0x000F;
static const uint HDR_OFFSET = 12;
static const int BUFFER_SIZE = 1024;

// for more info on DNS fields, see RFC 1035

struct queryHeader {
	uint id;			// 16 bit identifier assigned by the program who generated the query
    uint query;			// identifies if message is query (0) or a response (1)
    uint opcode;		// 4 bit field specifying kind of query: standard (0), inverse (1), server status request (2), reserved (3-15)
    uint aa;			// bit indicating if responding server is an authority for the domain name
    uint truncation;			// specifies whether this message was truncated or not
    uint recursionDesired;		// may be set in a query, directs the name server to pursue the query recursively
    uint recursionAvailable;	// set in a response, indicates whether recursive queries are available
    uint responseCode;			// 4 bit response code: 0 - no error, 1 - format error, 2 - server failure, 3 - name error, 4 - not implemented, 5 - refused
   
	// unsigned 16-bit integers
    uint questionCount;				// number of entries in the question section
    uint answerCount;				// number of resource reconds in the answer section
    uint nameServerCount;			// number of name server resource records in the authority records section
    uint additionalResourceCount;	// number of resource records in the additional records section
};

struct queryQuestion {
	std::string queryName;	// domain name requested
    uint queryType;			// 2 octet code specifying the type of the query
    uint queryClass;		// 2 octet code specifying the class of the query
};

queryHeader pQueryHeader;
queryQuestion pQueryQuestion;
std::map<std::string,std::vector<int>> resourceRecords;

uint get16bits(const char*& buffer) {
	uint value = static_cast<u_char> (buffer[0]);
    value = value << 8;
    value += static_cast<u_char> (buffer[1]);
    buffer += 2;
	
    return value;
}

void put8bits(char*& buffer, uint value) throw () {
    buffer[0] = (value);
    buffer += 1;
}

void put16bits(char*& buffer, uint value) throw () {
    buffer[0] = (value & 0xFF00) >> 8;
    buffer[1] = value & 0xFF;
    buffer += 2;
}

void put32bits(char*& buffer, ulong value) throw () {
    buffer[0] = (value & 0xFF000000) >> 24;
    buffer[1] = (value & 0xFF0000) >> 16;
    buffer[2] = (value & 0xFF00) >> 16;
    buffer[3] = (value & 0xFF) >> 16;
    buffer += 4;
}

queryHeader decodeHeader(const char* data, queryHeader pQueryHeader){
	pQueryHeader.id = get16bits(data);

    uint fields = get16bits(data);
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

queryQuestion decodeQuery(const char*& data, queryQuestion pQueryQuestion){
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
	if(bRRexists)
		fields += 0;			// Reply code
	else
		fields += 3;
    put16bits(buffer, fields);

    put16bits(buffer, pQueryHeader.questionCount);		// Questions count
	if(bRRexists)
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
	if(bRRexists){
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

void Service::start()
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
	
	// get the first network interfaces of the unikernel
	auto& inet = Inet4::stack<0>();
	Expects(inet.is_configured());
	
	printf("Service IP address is %s\n", inet.ip_addr().str().c_str());
	
	const UDP::port_t port = SERVER_PORT;
	auto& sock = inet.udp().bind(port);

	sock.on_read(
	[&sock] (UDP::addr_t addr, UDP::port_t port, const char* data, size_t len){
				
		pQueryHeader = decodeHeader(data, pQueryHeader);
		data += HDR_OFFSET;
		pQueryQuestion = decodeQuery(data, pQueryQuestion);
		
		// debug output
		printf("####### INCOMMING REQUEST #######\n");
		printf("Getting UDP data from %s:%d\n", addr.str().c_str(), port);
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
		
		sock.sendto(addr, port, buffer, nbrBytes);
	});

	INFO("DNS Server", "Listening on port %d\n", port);
}