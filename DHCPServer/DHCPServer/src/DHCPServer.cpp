//============================================================================
// Name        : DHCPServer.cpp
// Author      : Surojit Sengupta
// Version     :
// Copyright   : All Rights Reserved 2014
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>

#include <errno.h>
#include <string.h>

#include <netinet/in.h>
using namespace std;

int main() {
log4cxx::PropertyConfigurator::configure("config/log.cfg");
	log4cxx::LoggerPtr pLogger = log4cxx::Logger::getLogger("SASA-DHCPSERVER");
	time_t mTimeNow = time(NULL);
	LOG4CXX_INFO(pLogger,"SASA DHCP Server Starting up - " << ctime(&mTimeNow));

int s = socket(AF_INET,SOCK_DGRAM,0);

	if(s <0 )
	{
		LOG4CXX_ERROR(pLogger,"Failed to create udp socket - " << strerror(errno));
		return 0;
	}
	struct sockaddr_in SocketAddr;
	SocketAddr.sin_addr.s_addr=0;
	SocketAddr.sin_family=AF_INET;
	SocketAddr.sin_port=htons(67);

 	if (bind(s,(struct sockaddr*) &SocketAddr,sizeof(sockaddr_in)) < 0){

 		LOG4CXX_FATAL(pLogger," bind failed - " << strerror(errno));
 		return 0;
 	}
	return 0; // Bind Successful from Port 67
}
