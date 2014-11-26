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

class IPPoolServerCommunicator {
public:
	IPPoolServerCommunicator(std::string lServerIPAddress, int lServerPort,unsigned short lServerIdentifer);
	virtual ~IPPoolServerCommunicator();
	int getIpLease(std::string mac, std::string previousIp,unsigned int transactionId);
	int confirmIp(std::string mac, std::string ip);
	int releaseIp(std::string mac, std::string ip);
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
};

#endif /* IPPOOLSERVERCOMMUNICATOR_H_ */
