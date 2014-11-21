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
		int nOBytesRecd = recv(pParent->mClientSocket, pIncomingPacketBuffer,
				sizeof(RequestPacketPS), 0);
		LOG4CXX_DEBUG(pParent->pLogger,"Recd data packet from the pool");

		//Received a packet from the ip pool. It is placed in the pIncomingBuffer.
	}
return NULL;
}
