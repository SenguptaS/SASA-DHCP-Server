//============================================================================
// Name        : DHCPServer.cpp
// Author      : Surojit Sengupta
// Version     :
// Copyright   : All Rights Reserved 2014
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sys/types.h>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "DHCPPackets.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "IPPoolServerCommunicator.h"
#include <sstream>
#include "DHCPServerConstants.h"
#include <stdlib.h>

using namespace std;

enum REQUEST_TYPE {
	DHCP_DISCOVER = 1, DHCP_OFFER = 2, DHCP_REQUEST = 3, DHCP_NACK = 4
};

std::string lServerName;
std::string lIpServer;
std::string lServerPort;
std::string lInterfaceAddress;

int main(int argc, char* argv[]) {

	unsigned int lServerIdentifier = 0;

	//Initialize the logger from the log.cfg file
	log4cxx::PropertyConfigurator::configure("config/log.cfg");

	log4cxx::LoggerPtr pLogger = log4cxx::Logger::getLogger(ROOT_LOGGER);
	time_t mTimeNow = time(NULL);
	LOG4CXX_INFO(pLogger, "SASA DHCP Server Starting up - " << ctime(&mTimeNow));

	DiscoverPacket *pDiscoverPacket;

	std::string lClientMacAddress;
	std::string lPreviousIPAddress;
	unsigned long lClientTransactionID;
	REQUEST_TYPE lReqType;

	//Check if there are enough command line arguments that have been passed.
	if (argc < 8) {
		LOG4CXX_FATAL(pLogger,
				"Usage -n <server_identifier> -s <pool_server_ip> -p <pool_server_port> -i <interface_ip_address>");
		//Print required command line parameters
		return 0;
	}

	//Parse and store the command line arguments
	for (int x = 1; x < argc; x++) {

		if (strcmp(argv[x], "-n") == 0) {
			lServerIdentifier = strtoul(argv[x + 1], NULL, 10);
			if (errno == EINVAL) {
				LOG4CXX_FATAL(pLogger,
						"Please enter a numeric server identifier that will uniquely identify this server instance");
				return EXIT_FAILURE;
			}

		} else if (strcmp(argv[x], "-lServerUDPSocket") == 0) {
			lIpServer.assign(argv[x + 1]);
		} else if (strcmp(argv[x], "-p") == 0) {
			lServerPort.assign(argv[x + 1]);
		} else if (strcmp(argv[x], "-i") == 0) {
			lInterfaceAddress.assign(argv[x + 1]);
		} else {
			LOG4CXX_ERROR(pLogger, " Unknown Opcode :");
		}
		x++;
	}

	//Start the ip pool sever communicator
	IPPoolServerCommunicator ipsc(lIpServer, strtoul(lServerPort.c_str(),NULL,10)  ,(unsigned short int) lServerIdentifier );

	int lServerUDPSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if (lServerUDPSocket < 0) {
		LOG4CXX_ERROR(pLogger,
				"Failed to create udp socket - " << strerror(errno));
		return 0;
	}

	struct sockaddr_in SocketAddr;
	SocketAddr.sin_addr.s_addr = 0;
	SocketAddr.sin_family = AF_INET;
	SocketAddr.sin_port = htons(67);  // Socket Created

	if (bind(lServerUDPSocket, (struct sockaddr*) &SocketAddr,
			sizeof(sockaddr_in)) < 0) {

		LOG4CXX_FATAL(pLogger, " bind failed - " << strerror(errno));
		return 0;
	}

	// Bind Successful from Port 67

	//	if (listen(s, 0)) {
//		LOG4CXX_FATAL(pLogger, "FAILED TO LISTEN ON PORT - " << strerror(errno));
//		return 0; // Port Listens for incoming Packets
//	}

	while (true) {
		// Waiting to receive data
		char buffer[1024];
		unsigned int lSizeOfStructure = sizeof(sockaddr_in);
		int lSizeOfPacketRecvd = recvfrom(lServerUDPSocket, buffer, 1024, 0,
				(struct sockaddr*) &SocketAddr, &lSizeOfStructure);

		if (lSizeOfPacketRecvd < 0) {
			LOG4CXX_FATAL(pLogger,
					"Failed to recv packet - " << strerror(errno));
			continue;
		} else if (lSizeOfPacketRecvd == 0) {
			LOG4CXX_FATAL(pLogger,
					"Failed to recv packet - Peer has closed the socket");
			continue;
		} else {
			LOG4CXX_INFO(pLogger,
					"Recvd a udp packet of length: " << lSizeOfPacketRecvd);

			for (int lCtr = 0; lCtr < lSizeOfPacketRecvd; lCtr++) {
				printf("0x%02X ", buffer[lCtr]);
			}

			pDiscoverPacket = (DiscoverPacket*) buffer;

			int bConsumed = 0;
			int bLeft = lSizeOfPacketRecvd - sizeof(DiscoverPacket);
			char* pOptionsPtr = &buffer[sizeof(DiscoverPacket)];
			LOG4CXX_DEBUG(pLogger,
					"bConsumed: " << bConsumed << " bLeft: " << bLeft << " Op:" << (int) *pOptionsPtr);

			if (pDiscoverPacket->mOpField == 1) {

				if (pDiscoverPacket->mServerAddress == 0) {
					LOG4CXX_INFO(pLogger,
							" DHCP Discover - Transaction ID : " <<(int) pDiscoverPacket->mTransactionId <<" ClientAddress :" << pDiscoverPacket->mClientAddress << "ClientHardwareAddress : " << (int) pDiscoverPacket ->mClientHardwareAddress[0] << ":" << (int) pDiscoverPacket ->mClientHardwareAddress[1] << ":" << (int) pDiscoverPacket ->mClientHardwareAddress[2] << ":" << (int) pDiscoverPacket ->mClientHardwareAddress[3] << ":" << (int) pDiscoverPacket ->mClientHardwareAddress[4] << ":" << (int) pDiscoverPacket ->mClientHardwareAddress[5]);

				} else {
					LOG4CXX_INFO(pLogger,
							" DHCP Request - Transaction ID : " <<(int) pDiscoverPacket->mTransactionId <<" ClientAddress :" << pDiscoverPacket->mClientAddress << "ClientHardwareAddress : "<< pDiscoverPacket->mClientHardwareAddress);
				}
			}

			else if (pDiscoverPacket->mOpField == 2) {
				LOG4CXX_INFO(pLogger,
						" DHCP ACK - Transaction ID : " <<(int) pDiscoverPacket->mTransactionId <<" ClientAddress :" << pDiscoverPacket->mClientAddress << "ClientHardwareAddress : "<< pDiscoverPacket->mClientHardwareAddress);
			}

			while (bConsumed < bLeft) {
				//Read and process variable length field of the incoming dhcp packet.
				//Options as mentioned in the  https://tools.ietf.org/html/rfc2132
				unsigned char bOption = *pOptionsPtr;
				if (bOption == 0xFF) {
					LOG4CXX_INFO(pLogger, "End of options variable field");
					break;
				}

				char bLen = *(pOptionsPtr + 1);
				LOG4CXX_INFO(pLogger,
						"GOT Option:" <<(int) bOption << " with Length: " << (int) bLen);

				if (bOption == 53) {
					char bData = *(pOptionsPtr + 2);

					if (bData == 1) {

						LOG4CXX_INFO(pLogger, " Value(1): DHCPDISCOVER");
						lReqType = DHCP_DISCOVER;

					} else if (bData == 2) {

						LOG4CXX_INFO(pLogger, " Value(2): DHCPOFFER");
					} else if (bData == 3) {

						LOG4CXX_INFO(pLogger, " Value(3): DHCPREQUEST");
						lReqType = DHCP_REQUEST;
					}

					else if (bData == 4) {

						LOG4CXX_INFO(pLogger, " Value(4): DHCPDECLINE");
					} else if (bData == 5) {

						LOG4CXX_INFO(pLogger, " Value(5): DHCPACK");
						lReqType = DHCP_NACK;
					} else if (bData == 6) {

						LOG4CXX_INFO(pLogger, " Value(6): DHCPNACK");
					} else if (bData == 7) {

						LOG4CXX_INFO(pLogger, " Value(7): DHCPRELEASE");
					}

					else {

						LOG4CXX_ERROR(pLogger, " UNKNOWN DHCP OPTION ");
					}
				}

				else if (bOption == 61) {
					char bType = *(pOptionsPtr + 2);
					char lClientMac[6];

					if (bType != 0x01) {
						LOG4CXX_ERROR(pLogger,
								"Client hardware type of " << bType << " is not yet supported");
						break;
					}

					memcpy(lClientMac, (const void*) (pOptionsPtr + 3), 6);
					lClientMacAddress.assign(lClientMac);

				} else if (bOption == 50) {

					sockaddr_in lClientRequestedIP;
					memcpy(&lClientRequestedIP.sin_addr.s_addr,
							(pOptionsPtr + 2), 4);
					LOG4CXX_INFO(pLogger,
							" Requested IP Address : "<< inet_ntoa( lClientRequestedIP.sin_addr));

					lPreviousIPAddress.assign(
							inet_ntoa(lClientRequestedIP.sin_addr));

				}

				else if (bOption == 12) {
					char lClientDomainName[64];
					memcpy((void*) lClientDomainName, (pOptionsPtr + 2), bLen);
					lClientDomainName[(int) bLen] = 0x00;
					LOG4CXX_INFO(pLogger, " Host Name : " <<lClientDomainName);
				}

				bConsumed += 2 + bLen;
				pOptionsPtr += 2 + bLen;
			}

			if (lReqType == DHCP_DISCOVER) {
				ipsc.getIpLease(lClientMacAddress, lPreviousIPAddress,
						pDiscoverPacket->mTransactionId);
			} else if (lReqType == DHCP_REQUEST) {
				ipsc.confirmIp(lClientMacAddress, lPreviousIPAddress);
			} else if (lReqType == DHCP_NACK) {
				ipsc.releaseIp(lClientMacAddress, lPreviousIPAddress);
			}

		}

// Send data back to client

		long int SendData = sendto(lServerUDPSocket, (const void*) buffer,
				sizeof(ResponsePacket), 0, (struct sockaddr*) &SocketAddr,
				sizeof(sockaddr_in));
		{
			if (SendData < 0) {

				LOG4CXX_FATAL(pLogger, " Error - " << strerror(errno));
				return 0;
			} else {
				LOG4CXX_INFO(pLogger,
						" Data Successfully send- " << strerror(errno));
				return 0;
			}
		}
	}

}

