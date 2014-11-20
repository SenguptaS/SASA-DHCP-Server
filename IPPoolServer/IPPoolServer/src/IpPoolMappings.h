/*
 * IpPoolMappings.h
 *
 *  Created on: Nov 14, 2014
 *      Author: aditi
 */

#ifndef IPPOOLMAPPINGS_H_
#define IPPOOLMAPPINGS_H_
#include <log4cxx/logger.h>
#include <string>
#include "databaseConnection.h"

using namespace std;

class IpPoolMappings {
public:

	IpPoolMappings();
	int insertMapping(string lMacAddress, string lIpAddress);
	int deleteMapping(string lMacAddress, string lIpAddress);
	int setBindingFlag();
	int updateLease();
	int deleteMappingAsLeaseExpires();
	bool checkValidity(string lMacAddress, string lIpAddress);

private:

	LoggerPtr mPLogger;
	databaseConnection mDbConnection;
	int long mId;
	string mMacAddress;
	string mIpAddress;
	char mFlag;
	int long mLeaseTime;
};

#endif /* IPPOOLMAPPINGS_H_ */
