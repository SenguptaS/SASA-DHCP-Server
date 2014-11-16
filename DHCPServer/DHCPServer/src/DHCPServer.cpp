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
#include <stdio.h>

#include <errno.h>
#include <string.h>

#include <netinet/in.h>
#include "DHCPPackets.h"

using namespace std;

int main() {
	log4cxx::PropertyConfigurator::configure("config/log.cfg");
	log4cxx::LoggerPtr pLogger = log4cxx::Logger::getLogger("SASA-DHCPSERVER");
	time_t mTimeNow = time(NULL);
	LOG4CXX_INFO(pLogger, "SASA DHCP Server Starting up - " << ctime(&mTimeNow));

	int s = socket(AF_INET, SOCK_DGRAM, 0);

	if (s < 0) {
		LOG4CXX_ERROR(pLogger,
				"Failed to create udp socket - " << strerror(errno));
		return 0;
	}

	struct sockaddr_in SocketAddr;
	SocketAddr.sin_addr.s_addr = 0;
	SocketAddr.sin_family = AF_INET;
	SocketAddr.sin_port = htons(67);  // Socket Created

	if (bind(s, (struct sockaddr*) &SocketAddr, sizeof(sockaddr_in)) < 0) {

		LOG4CXX_FATAL(pLogger, " bind failed - " << strerror(errno));
		return 0;
	}

	// Bind Successful from Port 67

//	if (listen(s, 0)) {
//		LOG4CXX_FATAL(pLogger, "FAILED TO LISTEN ON PORT - " << strerror(errno));
//		return 0; // Port Listens for incoming Packets
//	}

	// Waiting to receive data
	char buffer[1024];
	unsigned int lSizeOfStructure = sizeof(sockaddr_in);
	int lSizeOfPacketRecvd = recvfrom(s, buffer, 1024, 0,
			(struct sockaddr*) &SocketAddr, &lSizeOfStructure);
	if (lSizeOfPacketRecvd < 0) {
		LOG4CXX_FATAL(pLogger, "Failed to recv packet - " << strerror(errno));
	} else if (lSizeOfPacketRecvd == 0) {
		LOG4CXX_FATAL(pLogger,
				"Failed to recv packet - Peer has closed the socket");
	} else {
		LOG4CXX_FATAL(pLogger,
				"Recvd a udp packet of length: " << lSizeOfPacketRecvd);

		for (int lCtr = 0; lCtr < lSizeOfPacketRecvd; lCtr++) {
			printf("0x%02X ", buffer[lCtr]);
		}
		DiscoverPacket *pDiscoverPacket = (DiscoverPacket*) buffer;

		int bConsumed = 0;
		int bLeft = lSizeOfPacketRecvd - sizeof(DiscoverPacket);
		char* pOptionsPtr = &buffer[sizeof(DiscoverPacket)];
		LOG4CXX_DEBUG(pLogger,
				"bConsumed: " << bConsumed << " bLeft: " << bLeft << " Op:" << (int) *pOptionsPtr);

		while (bConsumed < bLeft) {
			unsigned char bOption = *pOptionsPtr;
			if(bOption == 0xFF )
			{
				LOG4CXX_INFO(pLogger,"End of options variable field");
				break;
			}

			char bLen = *(pOptionsPtr+1);
			LOG4CXX_INFO(pLogger,
					"GOT Option:" <<(int) bOption << " with Length: " << (int) bLen);
			bConsumed += 2 + bLen;
			pOptionsPtr += 2+bLen;
		}

		if (pDiscoverPacket->mOpField == 1) {

			if (pDiscoverPacket->mServerAddress == 0) {
				LOG4CXX_INFO(pLogger,
						" DHCP Discover - Transaction ID : " <<(int) pDiscoverPacket->mTransactionId <<" ClientAddress :" << pDiscoverPacket->mClientAddress << "ClientHardwareAddress : " <<
						(int) pDiscoverPacket->mClientHardwareAddress[0] << ":" <<
						(int) pDiscoverPacket->mClientHardwareAddress[1] << ":" <<
						(int) pDiscoverPacket->mClientHardwareAddress[2] << ":" <<
						(int) pDiscoverPacket->mClientHardwareAddress[3] << ":" <<
						(int) pDiscoverPacket->mClientHardwareAddress[4] << ":" <<
						(int) pDiscoverPacket->mClientHardwareAddress[5] );

			} else {
				LOG4CXX_INFO(pLogger,
						" DHCP Request - Transaction ID : " <<(int) pDiscoverPacket->mTransactionId <<" ClientAddress :" << pDiscoverPacket->mClientAddress << "ClientHardwareAddress : "<< pDiscoverPacket->mClientHardwareAddress);
			}
		}

		else if (pDiscoverPacket->mOpField == 2) {
			LOG4CXX_INFO(pLogger,
					" DHCP ACK - Transaction ID : " <<(int) pDiscoverPacket->mTransactionId <<" ClientAddress :" << pDiscoverPacket->mClientAddress << "ClientHardwareAddress : "<< pDiscoverPacket->mClientHardwareAddress);
		}
//		LOG4CXX_DEBUG(pLogger,lOss.str());
	}
}

