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

IPPoolServerCommunicator::IPPoolServerCommunicator(std::string lServerIPAddress,
		int lServerPort, unsigned short lServerIdentifer) {



	mServerIPAddress = lServerIPAddress;
	mServerPort = lServerPort;
	mServerIdentifier = lServerIdentifer;
}

IPPoolServerCommunicator::~IPPoolServerCommunicator() {
	// TODO Auto
}
int IPPoolServerCommunicator::getIpLease(std::string mac,
		std::string previousIp, unsigned int transactionId) {

	RequestPacketPS r;
	in_addr addr;
	inet_aton(previouRequestPacketPS rsIp.c_str(),&addr);
	memcpy(r.mSourceHardwareAddress,mac.c_str(),6);
	 r.mOpField=1;
	r.mRequestId = transactionId;
	r.mPreviousIP = addr.s_addr;
	r.mChecksum = 0;
	r.mServerId = mServerIdentifier;
	r.mProtocolType = 4;


	int s = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in remoteSocketInfo;
	struct hostent *hPtr;
	const char *remoteHost="localhost";
	hPtr = gethostbyname(remoteHost);
	memcpy((char *)&remoteSocketInfo.sin_addr,0,0);
	remoteSocketInfo.sin_family = AF_INET;
	remoteSocketInfo.sin_port = htons((u_short)68);
	int connect(socket, (struct sockaddr *)&remoteSocketInfo, sizeof(sockaddr_in));










	return 0;
}


int IPPoolServerCommunicator::confirmIp(std::string mac, std::string ip) {

	RequestPacketPS r;
	in_addr addr;
	inet_aton(previouRequestPacketPS rsIp.c_str(),&addr);
	memcpy(r.mSourceHardwareAddress,mac.c_str(),6);
	 r.mOpField=1;
	r.mRequestId = transactionId;
	r.mPreviousIP = addr.s_addr;
	r.mChecksum = 0;
	r.mServerId = mServerIdentifier;
	r.mProtocolType = 4;


	return 0;
}



int IPPoolServerCommunicator::releaseIp(std::string mac, std::string ip) {


	RequestPacketPS r;
	in_addr addr;
	inet_aton(previouRequestPacketPS rsIp.c_str(),&addr);
	memcpy(r.mSourceHardwareAddress,mac.c_str(),6);
	 r.mOpField=1;
	r.mRequestId = transactionId;
	r.mPreviousIP = addr.s_addr;
	r.mChecksum = 0;
	r.mServerId = mServerIdentifier;
	r.mProtocolType = 4;

	return 0;

}


