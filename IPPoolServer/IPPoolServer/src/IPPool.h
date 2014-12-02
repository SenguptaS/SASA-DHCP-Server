/*
 * IPPool.h
 *
 *  Created on: Nov 14, 2014
 *      Author: surojit
 */

#ifndef IPPOOL_H_
#define IPPOOL_H_

#include <string>
#include "databaseConnection.h"

class IPPool {
private:
	databaseConnection mDatabaseConnection;
	log4cxx::LoggerPtr pLogger;

public:
	IPPool(Settings lSettings);
	virtual ~IPPool();
	int InitializePool();
	int AddIPRange(std::string lStartingAddress,unsigned int nNumberOfIPS );
	int SetHoles(std::string lStartingAddress,unsigned int nNumberOfIPS );
	int SetAvailable(std::string lStartingAddress,unsigned int nNumberOfIPS );
	int GetNextFreeIP(std::string& lNextFreeIP);
	int ReleaseIP(std::string lIPAddress);
	int SetIPAddressState(std::string lIPAddress, bool lInUse);
	bool isIpAvailable(std::string lIPAddress);

};

#endif /* IPPOOL_H_ */
