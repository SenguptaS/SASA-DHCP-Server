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
#include <csignal>
#include <signal.h>
#include <log4cxx/logger.h>
#include "PoolResponse.h"

class IPPoolServerCommunicator {
public:
	IPPoolServerCommunicator(std::string lServerIPAddress, int lServerPort,unsigned short lServerIdentifer,PoolResponse *pResponse);
	virtual ~IPPoolServerCommunicator();
	int getIpLease(char* mac, std::string previousIp,unsigned int transactionId);
	int confirmIp(char* mac, std::string ip, unsigned int transactionID);
	int releaseIp(char* mac, std::string ip, unsigned int transactionID);
	int InfoRequest(char* mac, std::string ip, unsigned int transactionID);
	int Run();
	int Stop();
private:
	std::string mServerIPAddress;
	int mServerPort;
	unsigned short mServerIdentifier;
	int mClientSocket;
	unsigned int mRun;
	static void* ResponseCommunicatorThread(void *pParams);
	log4cxx::LoggerPtr pLogger;
	PoolResponse *mpResponse;
};

#endif /* IPPOOLSERVERCOMMUNICATOR_H_ */
