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
#include <errno.h>

PoolResponse::PoolResponse(unsigned short nServerIdentifier,
		std::string nInterfaceIpAddress, int nUDPSocket) {
	pLogger = log4cxx::Logger::getLogger("PoolResponse");
	lServerIdentifier = nServerIdentifier;
	lLocalServerIPAddress = nInterfaceIpAddress;
	lUDPSocket = nUDPSocket;
}

int PoolResponse::ProcessIPOffer(SASA_responsePacket* pResponsePacket,
		std::string nInterfaceIpAddress, int nUDPSocket) //< SASA POOL PACKET >)
		{

	ResponsePacket lResponsePacket;
	memset(&lResponsePacket, 0, sizeof(ResponsePacket));

	lResponsePacket.mOpField = pResponsePacket->mOpField;
	memcpy(&lResponsePacket.mClientHardwareAddress,
			&pResponsePacket->mSrcHwAddress, 6);
	lResponsePacket.mFlags = 0x0100;
	lResponsePacket.mTransactionId = pResponsePacket->mRequestId;
	lResponsePacket.mGatewayAddress = pResponsePacket->mGatewayIp;
	lResponsePacket.mYourAddress = pResponsePacket->mAllocatedIp;
	lResponsePacket.mHeaderLength = 0x06;
	lResponsePacket.mHeaderType = 0x01;
	lResponsePacket.mServerAddress = inet_addr(lLocalServerIPAddress.c_str());
	lResponsePacket.mMagicCookie[0] = 0x63;
	lResponsePacket.mMagicCookie[1] = 0x82;
	lResponsePacket.mMagicCookie[2] = 0x53;
	lResponsePacket.mMagicCookie[3] = 0x63;

	unsigned int lTotalBytes = 0;

	char buffer[1024];
	memcpy((void*) buffer, (const void*) &lResponsePacket,
			sizeof(ResponsePacket));

	char* pOptionsPtr = &buffer[sizeof(ResponsePacket)];


	OPHeader *pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 53;
	pOPHeader->OPLength = 1;
	*(pOptionsPtr+2) = 0x02;
	pOptionsPtr += 3;


	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 51;
	pOPHeader->OPLength = 4;
	memcpy((pOPHeader + 2),
			(const char*) &pResponsePacket->mAllocationValidTime, 4);
	pOptionsPtr += 4 + 2;

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 1;
	pOPHeader->OPLength = 4;
	memcpy((pOPHeader + 2), (const char*) &pResponsePacket->mSubnetMask, 4);
	pOptionsPtr += 2 + 4;

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 255;
	pOPHeader->OPLength = 1;
	pOptionsPtr += 2;

	lTotalBytes = (pOptionsPtr - buffer);

	sockaddr_in lSockaddress;
	memset(&lSockaddress, 0, sizeof(sockaddr_in));
	lSockaddress.sin_addr.s_addr = inet_addr("255.255.255.255");
	lSockaddress.sin_family = AF_INET;
	lSockaddress.sin_port = htons(68);

	int lBytesSent = sendto(this->lUDPSocket, buffer, lTotalBytes, 0,
			(sockaddr*)&lSockaddress, sizeof(sockaddr_in));

	if(lBytesSent < 0)
	{
		LOG4CXX_ERROR(pLogger,"Failed to send dhcp offer to client - " << strerror(errno));
		return 0;
	}
	in_addr lInAddr;
	lInAddr.s_addr = pResponsePacket->mAllocatedIp;

	LOG4CXX_INFO(pLogger,
			"Sent " << lBytesSent << " to client - " << inet_ntoa(lInAddr) );
	return lBytesSent;
}

PoolResponse::~PoolResponse() {
	// TODO Auto-generated destructor stub
}

