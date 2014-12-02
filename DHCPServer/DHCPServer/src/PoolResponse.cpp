/*
 * PoolResponse.cpp
 *
 *  Created on: Nov 20, 2014
 *      Author: ateiv
 */

#include "PoolResponse.h"

#include <arpa/inet.h>
#include <errno.h>
#include <log4cxx/helpers/messagebuffer.h>
#include <log4cxx/helpers/objectptr.h>
#include <log4cxx/level.h>
#include <log4cxx/spi/location/locationinfo.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "DHCPPackets.h"
#include "RequestPacketHolder.h"

//#include "sasaPackets.h"

int PoolResponse::SendNAK(SASA_responsePacket* pResponsePacket,
		std::string nInterfaceIpAddress, int nUDPSocket) {
	ResponsePacket lResponsePacket;
	memset(&lResponsePacket, 0, sizeof(ResponsePacket));

	lResponsePacket.mOpField = 0x02; // pResponsePacket->mOpField;
	memcpy(&lResponsePacket.mClientHardwareAddress,
			&pResponsePacket->mSrcHwAddress, 6);
	lResponsePacket.mFlags = 0x80;
	lResponsePacket.mTransactionId = pResponsePacket->mRequestId;
	lResponsePacket.mGatewayAddress = 0x00; // pResponsePacket->mGatewayIp;
	lResponsePacket.mYourAddress = 0x00;
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
	*(pOptionsPtr + 2) = 0x06;
	pOptionsPtr += 3;

	unsigned int lServerIdentifier = inet_addr(lLocalServerIPAddress.c_str());

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 54;
	pOPHeader->OPLength = 4;
	memcpy((void*) (pOptionsPtr + 2), &lServerIdentifier, 4);
	pOptionsPtr += 6;

	const std::string lErrorMessage = "Requested address not available";

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 56;
	pOPHeader->OPLength = lErrorMessage.length();
	memcpy((void*) (pOptionsPtr + 2), lErrorMessage.c_str(),
			lErrorMessage.length());
	pOptionsPtr += 2 + lErrorMessage.length();

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 255;
	pOPHeader->OPLength = 1;
	pOptionsPtr += 1;

	lTotalBytes = (pOptionsPtr - buffer);

	sockaddr_in lSockaddress;
	memset(&lSockaddress, 0, sizeof(sockaddr_in));
	lSockaddress.sin_addr.s_addr = inet_addr("255.255.255.255");
	lSockaddress.sin_family = AF_INET;
	lSockaddress.sin_port = htons(68);

	int lBytesSent = 0;
	for (int x = 0; x < 3; x++) {
		lBytesSent = sendto(this->lUDPSocket, buffer, lTotalBytes, 0,
				(sockaddr*) &lSockaddress, sizeof(sockaddr_in));

		if (lBytesSent < 0) {
			LOG4CXX_ERROR(pLogger,
					"Failed to send dhcp offer to client - " << strerror(errno));
			return x;

		}

		::usleep(1e5);
	}

	LOG4CXX_INFO(pLogger,
			"Sent " << lBytesSent << " NAK to client transaction ID - " << pResponsePacket->mRequestId);
	return lBytesSent;
}

PoolResponse::PoolResponse(unsigned short nServerIdentifier,
		std::string nInterfaceIpAddress, int nUDPSocket,
		TransactionIDMapper *pTransactionMapper) {
	pLogger = log4cxx::Logger::getLogger("PoolResponse");
	lServerIdentifier = nServerIdentifier;
	lLocalServerIPAddress = nInterfaceIpAddress;
	lUDPSocket = nUDPSocket;
	this->lpTransactionMapper = pTransactionMapper;
}

int PoolResponse::SendACK(SASA_responsePacket* pResponsePacket,
		std::string nInterfaceIpAddress, int nUDPSocket) {

	RequestPacketHolder* pRequestPacket =
			lpTransactionMapper->GetPacketForTransactionID(
					pResponsePacket->mRequestId);
	if (pRequestPacket != NULL) {
		lpTransactionMapper->RemovePacketHolder(pResponsePacket->mRequestId);
		delete pRequestPacket;
	}

	ResponsePacket lResponsePacket;
	memset(&lResponsePacket, 0, sizeof(ResponsePacket));

	lResponsePacket.mOpField = 0x02; // pResponsePacket->mOpField;
	memcpy(&lResponsePacket.mClientHardwareAddress,
			&pResponsePacket->mSrcHwAddress, 6);
	lResponsePacket.mFlags = 0x80;
	lResponsePacket.mTransactionId = pResponsePacket->mRequestId;
	lResponsePacket.mGatewayAddress = 0x00; // pResponsePacket->mGatewayIp;
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
	memcpy((void*) (pOptionsPtr + 2), (const void*) &pResponsePacket->mDnsIp,
			4);
	pOptionsPtr += 2 + 4;

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 255;
	pOPHeader->OPLength = 1;
	pOptionsPtr += 1;

	lTotalBytes = (pOptionsPtr - buffer);

	sockaddr_in lSockaddress;
	memset(&lSockaddress, 0, sizeof(sockaddr_in));
	lSockaddress.sin_addr.s_addr = inet_addr("255.255.255.255");
	lSockaddress.sin_family = AF_INET;
	lSockaddress.sin_port = htons(68);

	int lBytesSent = 0;
	for (int x = 0; x < 3; x++) {

		lBytesSent = sendto(this->lUDPSocket, buffer, lTotalBytes, 0,
				(sockaddr*) &lSockaddress, sizeof(sockaddr_in));

		if (lBytesSent < 0) {
			LOG4CXX_ERROR(pLogger,
					"Failed to send dhcp offer to client - " << strerror(errno));
			return 0;
		}
		::usleep(1e5);
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

	RequestPacketHolder* pRequestPacket =
			lpTransactionMapper->GetPacketForTransactionID(
					pResponsePacket->mRequestId);
	if (pRequestPacket != NULL) {
		if (pRequestPacket->getClientRequestedIp().compare("0.0.0.0") == 0
				|| pRequestPacket->getClientRequestedIp().compare("") == 0) {
			lpTransactionMapper->RemovePacketHolder(
					pResponsePacket->mRequestId);
			delete pRequestPacket;
		} else {
			std::string lFormattedIP;
			in_addr lInaddr;
			lInaddr.s_addr = pResponsePacket->mAllocatedIp;
			lFormattedIP.assign(inet_ntoa(lInaddr));

			if (pRequestPacket->getClientRequestedIp().compare(lFormattedIP)
					== 0) {
				lpTransactionMapper->RemovePacketHolder(
						pResponsePacket->mRequestId);
				delete pRequestPacket;
			} else {

				lpTransactionMapper->RemovePacketHolder(
						pResponsePacket->mRequestId);

				delete pRequestPacket;

				this->SendNAK(pResponsePacket, nInterfaceIpAddress, nUDPSocket);
//				return 0;
			}
		}
	}
	ResponsePacket lResponsePacket;
	memset(&lResponsePacket, 0, sizeof(ResponsePacket));

	lResponsePacket.mOpField = pResponsePacket->mOpField;
	memcpy(&lResponsePacket.mClientHardwareAddress,
			&pResponsePacket->mSrcHwAddress, 6);
	lResponsePacket.mFlags = 0x80;
	lResponsePacket.mTransactionId = pResponsePacket->mRequestId;
	lResponsePacket.mGatewayAddress = 0x00; // pResponsePacket->mGatewayIp;
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
	memcpy((void*) (pOptionsPtr + 2), (const void*) &pResponsePacket->mDnsIp,
			4);
	pOptionsPtr += 2 + 4;

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 255;
	pOPHeader->OPLength = 1;
	pOptionsPtr += 1;

	lTotalBytes = (pOptionsPtr - buffer);

	sockaddr_in lSockaddress;
	memset(&lSockaddress, 0, sizeof(sockaddr_in));
	lSockaddress.sin_addr.s_addr = inet_addr("255.255.255.255");
	lSockaddress.sin_family = AF_INET;
	lSockaddress.sin_port = htons(68);

	int lBytesSent = 0;
	for (int x = 0; x < 3; x++) {
		lBytesSent = sendto(this->lUDPSocket, buffer, lTotalBytes, 0,
				(sockaddr*) &lSockaddress, sizeof(sockaddr_in));

		if (lBytesSent < 0) {
			LOG4CXX_ERROR(pLogger,
					"Failed to send dhcp offer to client - " << strerror(errno));
			return x;
		}
		::usleep(1e5);
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

