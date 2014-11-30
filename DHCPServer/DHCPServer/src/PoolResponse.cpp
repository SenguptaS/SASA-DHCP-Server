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

int PoolResponse::SendACK(SASA_responsePacket* pResponsePacket,std::string nInterfaceIpAddress, int nUDPSocket)
{
	ResponsePacket lResponsePacket;
	memset(&lResponsePacket, 0, sizeof(ResponsePacket));

	lResponsePacket.mOpField = 0x02;// pResponsePacket->mOpField;
	memcpy(&lResponsePacket.mClientHardwareAddress,
			&pResponsePacket->mSrcHwAddress, 6);
	lResponsePacket.mFlags = 0x0100;
	lResponsePacket.mTransactionId = pResponsePacket->mRequestId;
	lResponsePacket.mGatewayAddress = 0x00;// pResponsePacket->mGatewayIp;
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
	*(pOptionsPtr + 2) = 0x05;
	pOptionsPtr += 3;

	unsigned int lServerIdentifier = inet_addr(lLocalServerIPAddress.c_str());

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 54;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2), &lServerIdentifier, 4);
	pOptionsPtr += 6;

	unsigned int lLeaseTime, lSubnetMask;

	lLeaseTime = pResponsePacket->mAllocationValidTime;
	lLeaseTime = htonl(lLeaseTime);

	lSubnetMask = pResponsePacket->mSubnetMask;
	//lSubnetMask = htonl(lSubnetMask);

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 51;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2), (const void*) &lLeaseTime, 4);
	pOptionsPtr += 4 + 2;

	unsigned int lRenewalTime = (lLeaseTime / 2);

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 58;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2), (const void*) &lRenewalTime, 4);
	pOptionsPtr += 4 + 2;

	unsigned int lRebindingTime = htonl(ntohl(lLeaseTime) * 0.875f);

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 59;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2), (const void*) &lRebindingTime, 4);
	pOptionsPtr += 4 + 2;

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 1;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2), (const void*) &lSubnetMask, 4);
	pOptionsPtr += 2 + 4;

	unsigned int lBroadcastAddress = pResponsePacket->mAllocatedIp | 0xFF000000;
	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 28;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2), (const void*) &lBroadcastAddress, 4);
	pOptionsPtr += 2 + 4;

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 3;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2),
			(const void*) &pResponsePacket->mGatewayIp, 4);
	pOptionsPtr += 2 + 4;

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 6;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2),
			(const void*) &pResponsePacket->mDnsIp, 4);
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
			(sockaddr*) &lSockaddress, sizeof(sockaddr_in));

	if (lBytesSent < 0) {
		LOG4CXX_ERROR(pLogger,
				"Failed to send dhcp offer to client - " << strerror(errno));
		return 0;
	}
	in_addr lInAddr;
	lInAddr.s_addr = pResponsePacket->mAllocatedIp;

	LOG4CXX_INFO(pLogger,
			"Sent " << lBytesSent << " to client - " << inet_ntoa(lInAddr));
	return lBytesSent;
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
	lResponsePacket.mGatewayAddress = 0x00;// pResponsePacket->mGatewayIp;
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
	*(pOptionsPtr + 2) = 0x02;
	pOptionsPtr += 3;

	unsigned int lServerIdentifier = inet_addr(lLocalServerIPAddress.c_str());

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 54;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2), &lServerIdentifier, 4);
	pOptionsPtr += 6;

	unsigned int lLeaseTime, lSubnetMask;

	lLeaseTime = pResponsePacket->mAllocationValidTime;
	lLeaseTime = htonl(lLeaseTime);

	lSubnetMask = pResponsePacket->mSubnetMask;
	//lSubnetMask = htonl(lSubnetMask);

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 51;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2), (const void*) &lLeaseTime, 4);
	pOptionsPtr += 4 + 2;

	unsigned int lRenewalTime = (lLeaseTime / 2);

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 58;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2), (const void*) &lRenewalTime, 4);
	pOptionsPtr += 4 + 2;

	unsigned int lRebindingTime = htonl(ntohl(lLeaseTime) * 0.875f);

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 59;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2), (const void*) &lRebindingTime, 4);
	pOptionsPtr += 4 + 2;

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 1;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2), (const void*) &lSubnetMask, 4);
	pOptionsPtr += 2 + 4;

	unsigned int lBroadcastAddress = pResponsePacket->mAllocatedIp | 0xFF000000;
	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 28;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2), (const void*) &lBroadcastAddress, 4);
	pOptionsPtr += 2 + 4;

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 3;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2),
			(const void*) &pResponsePacket->mGatewayIp, 4);
	pOptionsPtr += 2 + 4;

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 6;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2),
			(const void*) &pResponsePacket->mDnsIp, 4);
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
			(sockaddr*) &lSockaddress, sizeof(sockaddr_in));

	if (lBytesSent < 0) {
		LOG4CXX_ERROR(pLogger,
				"Failed to send dhcp offer to client - " << strerror(errno));
		return 0;
	}
	in_addr lInAddr;
	lInAddr.s_addr = pResponsePacket->mAllocatedIp;

	LOG4CXX_INFO(pLogger,
			"Sent " << lBytesSent << " to client - " << inet_ntoa(lInAddr));
	return lBytesSent;
}

PoolResponse::~PoolResponse() {
	// TODO Auto-generated destructor stub
}

