/*
 * IPPoolServerCommunicator.cpp
 *
 *  Created on: Nov 16, 2014
 *      Author: sumit
 */

#include "IPPoolServerCommunicator.h"

#include <arpa/inet.h>
#include <errno.h>
#include <log4cxx/helpers/messagebuffer.h>
#include <log4cxx/helpers/objectptr.h>
#include <log4cxx/level.h>
#include <log4cxx/spi/location/locationinfo.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>

#include "DHCPPackets.h"
#include "DHCPServerConstants.h"
#include "sasaPackets.h"

IPPoolServerCommunicator::IPPoolServerCommunicator(std::string lServerIPAddress,
		int lServerPort, unsigned short lServerIdentifer,
		PoolResponse *pResponse) {

	mServerIPAddress = lServerIPAddress;
	mServerPort = lServerPort;
	mServerIdentifier = lServerIdentifer;
	mRun = 0;
	mClientSocket = 0;
	mpResponse = pResponse;
	this->pLogger = log4cxx::Logger::getLogger(ROOT_LOGGER);

}

int IPPoolServerCommunicator::Run() {
	if (this->mRun) {
		return 1;
	} else {
		this->mRun = 1;
		pthread_t lPthreadT;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		int error_number = pthread_create(&lPthreadT, &attr,
				IPPoolServerCommunicator::ResponseCommunicatorThread, this);
	}
	return this->mRun;
}

int IPPoolServerCommunicator::Stop() {
	this->mRun = 0;
	::sleep(1);
	return 0;
}

IPPoolServerCommunicator::~IPPoolServerCommunicator() {
	// TODO Auto
}
int IPPoolServerCommunicator::getIpLease(char* mac, std::string previousIp,
		unsigned int transactionId) {

	RequestPacketPS r;
	in_addr addr;
	if (previousIp.length() < 6) {
		addr.s_addr = 0;
	} else {
		inet_aton(previousIp.c_str(), &addr);
	}

	memcpy(r.mSourceHardwareAddress, mac, 6);
	r.mOpField = 1;
	r.mRequestId = transactionId;
	r.mPreviousIP = addr.s_addr;
	r.mChecksum = 0;
	r.mServerId = mServerIdentifier;
	r.mProtocolType = 4;

	//Send the packet to the IP Pool server and recv a response
	int bSent = send(this->mClientSocket, &r, sizeof(RequestPacketPS), 0);
	if (bSent < 0) {
		LOG4CXX_ERROR(pLogger,
				"Failed sending request to IP Pool server " << strerror(errno));
		return 0;
	} else {
		LOG4CXX_ERROR(pLogger, "Sent request to ip pool");
		return 1;
	}

	return 0;
}

int IPPoolServerCommunicator::confirmIp(char* mac, std::string ip,
		unsigned int ltransactionID) {

	RequestPacketPS r;
	in_addr addr;
	if (ip.length() < 6) {
		addr.s_addr = 0;
	} else {
		inet_aton(ip.c_str(), &addr);
	}

	memcpy(r.mSourceHardwareAddress, mac, 6);
	r.mOpField = 3;
	r.mRequestId = ltransactionID;
	r.mPreviousIP = addr.s_addr;
	r.mChecksum = 0;
	r.mServerId = mServerIdentifier;
	r.mProtocolType = 4;

	//Send the packet to the IP Pool server and recv a response
	int bSent = send(this->mClientSocket, &r, sizeof(RequestPacketPS), 0);
	if (bSent < 0) {
		LOG4CXX_ERROR(pLogger,
				"Failed sending request to IP Pool server " << strerror(errno));
		return 0;
	} else {
		LOG4CXX_ERROR(pLogger, "Sent IP confirmation to pool");
		return 1;
	}

	return 0;
}

int IPPoolServerCommunicator::releaseIp(char* mac, std::string ip,
		unsigned int transactionID) {

	RequestPacketPS r;
	in_addr addr;
	inet_aton(ip.c_str(), &addr);
	memcpy(r.mSourceHardwareAddress, mac, 6);
	r.mOpField = 5;
	r.mRequestId = transactionID;
	r.mPreviousIP = addr.s_addr;
	r.mChecksum = 0;
	r.mServerId = mServerIdentifier;
	r.mProtocolType = 4;

	//Send the packet to the IP Pool server and recv a response
	int bSent = send(this->mClientSocket, &r, sizeof(RequestPacketPS), 0);
	if (bSent < 0) {
		LOG4CXX_ERROR(pLogger,
				"Failed sending request to IP Pool server " << strerror(errno));
		return 0;
	} else {
		LOG4CXX_ERROR(pLogger, "Sent IP Release to pool");
		return 1;
	}

	return 0;
}

int IPPoolServerCommunicator::InfoRequest(char* mac, std::string ip,
		unsigned int transactionID) {

	RequestPacketPS r;
	in_addr addr;
	inet_aton(ip.c_str(), &addr);
	memcpy(r.mSourceHardwareAddress, mac, 6);
	r.mOpField = 6;
	r.mRequestId = transactionID;
	r.mPreviousIP = addr.s_addr;
	r.mChecksum = 0;
	r.mServerId = mServerIdentifier;
	r.mProtocolType = 4;

	//Send the packet to the IP Pool server and recv a response
	int bSent = send(this->mClientSocket, &r, sizeof(RequestPacketPS), 0);
	if (bSent < 0) {
		LOG4CXX_ERROR(pLogger,
				"Failed sending request to IP Pool server " << strerror(errno));
		return 0;
	} else {
		LOG4CXX_ERROR(pLogger, "Sent IP Info Request to pool");
		return 1;
	}

	return 0;
}

void* IPPoolServerCommunicator::ResponseCommunicatorThread(void *pParams) {
	IPPoolServerCommunicator* pParent = (IPPoolServerCommunicator*) pParams;
	char pIncomingPacketBuffer[1024];
	log4cxx::LoggerPtr lLogger = log4cxx::Logger::getLogger(
			"ResponseCommunicatorThread");
	while (pParent->mRun) {

		pParent->mClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (pParent->mClientSocket < 0) {
			LOG4CXX_ERROR(lLogger,
					"Failed to create pool socket - " << strerror(errno));
			return 0x00;
		}

		sockaddr_in lAddress;
		memset(&lAddress, 0, sizeof(sockaddr_in));

		lAddress.sin_addr.s_addr = inet_addr(pParent->mServerIPAddress.c_str());
		lAddress.sin_family = AF_INET;
		lAddress.sin_port = htons(pParent->mServerPort);
		LOG4CXX_INFO(lLogger,
				"Connecting to IPPool Server on " << pParent->mServerIPAddress << " : " << pParent->mServerPort);
		int lRetVal = connect(pParent->mClientSocket, (sockaddr*) &lAddress,
				sizeof(sockaddr_in));

		if (lRetVal < 0) {
			LOG4CXX_ERROR(lLogger,
					"Failed to connect to IP Pool server : " << strerror(errno));
			close(pParent->mClientSocket);
			::sleep(5);
			continue;
		}

		LOG4CXX_ERROR(lLogger, "Connected to the ip pool server successfully");

		while (true) {
			memset(pIncomingPacketBuffer, 0x00, 1024);
			int nOBytesRecd = recv(pParent->mClientSocket,
					pIncomingPacketBuffer, sizeof(SASA_responsePacket), 0);

			if (nOBytesRecd == 0) {
				LOG4CXX_ERROR(lLogger,
						"IP Pool server has closed the connection");
				break;
			}

			if (nOBytesRecd < 0) {
				LOG4CXX_ERROR(lLogger, "Connection error - " << strerror(errno));
				break;
			}

			SASA_responsePacket *pPoolResponsePacket =
					(SASA_responsePacket *) pIncomingPacketBuffer;
			if (pPoolResponsePacket->mOpField == 2) {
				if( pParent->mpResponse->ProcessIPOffer(pPoolResponsePacket,
						pParent->mServerIPAddress, pParent->mClientSocket) <= 0)
				{
					in_addr lIp;
					lIp.s_addr = pPoolResponsePacket->mAllocatedIp;
					std::string lFormattedIP;
					lFormattedIP.assign(inet_ntoa(lIp));
					pParent->releaseIp((char*) pPoolResponsePacket->mSrcHwAddress,lFormattedIP,pPoolResponsePacket->mRequestId);
				}
			} else if (pPoolResponsePacket->mOpField == 4) {
				pParent->mpResponse->SendACK(pPoolResponsePacket,
						pParent->mServerIPAddress, pParent->mClientSocket);
			} else if (pPoolResponsePacket->mOpField == 6) {
				pParent->mpResponse->SendACK(pPoolResponsePacket,
						pParent->mServerIPAddress, pParent->mClientSocket);
			}

			//Received a packet from the ip pool. It is placed in the pIncomingBuffer.

		}
	}
	return NULL;
}
