/*
 * PoolResponse.cpp
 *
 *  Created on: Nov 20, 2014
 *      Author: ateiv
 */

#include<string.h>
#include "DHCPPackets.h"
#include<sys/types.h>
#include<ifaddrs.h>
#include <log4cxx/logger.h>
#include<sys/socket.h>
#include<netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "PoolResponse.h"
#include "sasaPackets.h"

PoolResponse::PoolResponse(unsigned short nServerIdentifier,
		std::string nInterfaceIpAddress, int nUDPSocket) {
	pLogger = log4cxx::Logger::getLogger("PoolResponse");
	lServerIdentifier = nServerIdentifier;
	lLocalServerIPAddress = nInterfaceIpAddress;
	lUDPSocket = nUDPSocket;
}

int PoolResponse::ProcessIPOffer(const SASA_responsePacket* pResponsePacket,
		std::string nInterfaceIpAddress, int nUDPSocket) //< SASA POOL PACKET >)
		{

	ResponsePacket lResponsePacket;
	memset(&lResponsePacket, 0, sizeof(ResponsePacket));

	lResponsePacket.mOpField = pResponsePacket->mOpField;
	memcpy(&lResponsePacket.mClientHardwareAddress,
			&pResponsePacket->mSrcHwAddress, 6);
	lResponsePacket.mTransactionId = pResponsePacket->mRequestId;
	lResponsePacket.mGatewayAddress = pResponsePacket->mGatewayIp;
	lResponsePacket.mYourAddress = pResponsePacket->mAllocatedIp;
	lResponsePacket.mHeaderLength = 0x06;
	lResponsePacket.mHeaderType = 0x01;
	lResponsePacket.mServerAddress = inet_addr(lLocalServerIPAddress.c_str());

	unsigned int lTotalBytes = 0;

	char buffer[1024];
	memcpy((void*) buffer, (const void*) &lResponsePacket,
			sizeof(ResponsePacket));

	char* pOptionsPtr = &buffer[sizeof(ResponsePacket)];

	OPHeader *pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 51;
	pOPHeader->OPLength = 4;
	memcpy((&pOPHeader + 2),
			(const char*) &pResponsePacket->mAllocationValidTime, 4);
	pOptionsPtr += 4 + 2;

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 1;
	pOPHeader->OPLength = 4;
	memcpy((&pOPHeader + 2), (const char*) &pResponsePacket->mSubnetMask, 4);
	pOptionsPtr += 2 + 4;

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 255;
	pOPHeader->OPLength = 1;
	pOptionsPtr += 2;

	lTotalBytes = sizeof(ResponsePacket) + (pOptionsPtr - buffer);

	sockaddr_in lSockaddress;
	memset(&lSockaddress, 0, sizeof(sockaddr_in));
	lSockaddress.sin_addr.s_addr = inet_addr("255.255.255.255");
	lSockaddress.sin_family = AF_INET;
	lSockaddress.sin_port = htons(68);

	int lBytesSent = sendto(this->lUDPSocket, buffer, lTotalBytes, 0,
			(sockaddr*)&lSockaddress, sizeof(sockaddr_in));

	in_addr lInAddr;
	lInAddr.s_addr = pResponsePacket->mAllocatedIp;

	LOG4CXX_INFO(pLogger,
			"Sent " << lBytesSent << " to client - " << inet_ntoa(lInAddr) );
}

PoolResponse::~PoolResponse() {
	// TODO Auto-generated destructor stub
}

