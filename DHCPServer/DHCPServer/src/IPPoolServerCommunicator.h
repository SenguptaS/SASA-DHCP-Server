/*
 * IPPoolServerCommunicator.h
 *
 *  Created on: Nov 16, 2014
 *      Author: sumit
 */

#ifndef IPPOOLSERVERCOMMUNICATOR_H_
#define IPPOOLSERVERCOMMUNICATOR_H_
#include <string>
#include <stdio.h>

class IPPoolServerCommunicator {
public:
	IPPoolServerCommunicator(std::string lServerIPAddress, int lServerPort,unsigned short lServerIdentifer);
	virtual ~IPPoolServerCommunicator();
	int getIpLease(std::string mac, std::string previousIp,
			unsigned int transactionId, short int returnType);
	int confirmIp(std::string mac, std::string ip);
	int releaseIp(std::string mac, std::string ip);

private:
	std::string mServerIPAddress;
	int mServerPort;
	unsigned short mServerIdentifier;
};

#endif /* IPPOOLSERVERCOMMUNICATOR_H_ */
