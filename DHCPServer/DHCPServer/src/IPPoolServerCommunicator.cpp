/*
 * IPPoolServerCommunicator.cpp
 *
 *  Created on: Nov 16, 2014
 *      Author: sumit
 */

#include "IPPoolServerCommunicator.h"
#include <string>
#include <memory.h>
#include <sys/types.h>
#include "DHCPPackets.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <netdb.h>
#include <unistd.h>
#include "DHCPServerConstants.h"

IPPoolServerCommunicator::IPPoolServerCommunicator(std::string lServerIPAddress,
		int lServerPort, unsigned short lServerIdentifer) {

	mServerIPAddress = lServerIPAddress;
	mServerPort = lServerPort;
	mServerIdentifier = lServerIdentifer;
	mRun = 0;
	mClientSocket = 0;
	pLogger = log4cxx::Logger::getLogger(ROOT_LOGGER);

}

int IPPoolServerCommunicator::Run() {
	if (this->mRun) {
		return 1;
	} else {
		this->mRun = 1;
		pthread_t lPthreadT;
		pthread_attr_t attr;
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
int IPPoolServerCommunicator::getIpLease(std::string mac,
		std::string previousIp, unsigned int transactionId) {

	RequestPacketPS r;
	in_addr addr;
	inet_aton(previousIp.c_str(), &addr);
	memcpy(r.mSourceHardwareAddress, mac.c_str(), 6);
	r.mOpField = 1;
	r.mRequestId = transactionId;
	r.mPreviousIP = addr.s_addr;
	r.mChecksum = 0;
	r.mServerId = mServerIdentifier;
	r.mProtocolType = 4;

	//Send the packet to the IP Pool server and recv a response

	return 0;
}

int IPPoolServerCommunicator::confirmIp(std::string mac, std::string ip) {

	RequestPacketPS r;
	in_addr addr;
	inet_aton(ip.c_str(), &addr);
	memcpy(r.mSourceHardwareAddress, mac.c_str(), 6);
	r.mOpField = 2;
	r.mRequestId = 0;
	r.mPreviousIP = addr.s_addr;
	r.mChecksum = 0;
	r.mServerId = mServerIdentifier;
	r.mProtocolType = 4;

	return 0;
}

int IPPoolServerCommunicator::releaseIp(std::string mac, std::string ip) {

	RequestPacketPS r;
	in_addr addr;
	inet_aton(ip.c_str(), &addr);
	memcpy(r.mSourceHardwareAddress, mac.c_str(), 6);
	r.mOpField = 3;
	r.mRequestId = 0;
	r.mPreviousIP = addr.s_addr;
	r.mChecksum = 0;
	r.mServerId = mServerIdentifier;
	r.mProtocolType = 4;

	return 0;

}

void* IPPoolServerCommunicator::ResponseCommunicatorThread(void *pParams) {
	IPPoolServerCommunicator* pParent = (IPPoolServerCommunicator*) pParams;

	char pIncomingPacketBuffer[1024];
	while (pParent->mRun) {

		pParent->mClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (pParent->mClientSocket < 0) {
			LOG4CXX_EROR(pParent->pLogger,
					"Failed to create pool socket - " << strerror(errno));
			return 0x00;
		}

		sockaddr_in lAddress;
		memset(lAddress, 0, sizeof(sockaddr_in));

		lAddress.sin_addr = inet_addr(pParent->mServerIPAddress.c_str());
		lAddress.sin_family
		AF_INET;
		lAddress.sin_port = htons(pParent->mServerPort);
		LOG4CXX_INFO(pParent->pLogger,
				"Connecting to IPPool Server on " << pParent->mServerIPAddress << " : " << pParent->mServerPort);
		int lRetVal = connect(pParent->mClientSocket, lAddress,
				sizeof(sockaddr_in));

		if (lRetVal < 0) {
			LOG4CXX_ERROR(pLogger,
					"Failed to connect to IP Pool server : " << strerror(errno));
			close(pParent->mClientSocket);
			continue;
		}

		LOG4CXX_ERROR(pParent->pLogger,
				"Connected to the ip pool server successfully");

		while (true) {
			int nOBytesRecd = recv(pParent->mClientSocket,
					pIncomingPacketBuffer, sizeof(SASA_responsePacket), 0);

			if(nOBytesRecd == 0 )
			{
				LOG4CXX_ERROR(pParent->pLogger,"IP Pool server has closed the connection");
				break;
			}

			if(nOBytesRecd < 0)
			{
				LOG4CXX_ERROR(pParent->pLogger,"Connection error - " << strerror(errno));
				break;
			}

			SASA_responsePacket *pPoolResponsePacket =(SASA_responsePacket *) pIncomingPacketBuffer;
			pPoolResponsePacket->
			//Received a packet from the ip pool. It is placed in the pIncomingBuffer.
		}
	}
	return NULL;
}
