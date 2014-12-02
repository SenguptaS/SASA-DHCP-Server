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

	IpPoolMappings(const Settings &lSettings);
	int insertMapping(string lMacAddress, string lIpAddress);
	int deleteMapping(string lMacAddress, string lIpAddress);
	int setBindingFlag(string lMacAddress, string lIpAddress);
	int updateLease(string lMacAddress, string lIpAddress);
	int deleteMappingAsLeaseExpires();
	bool checkValidity(string lMacAddress, string lIpAddress);

private:

	LoggerPtr mPLogger;
	int long mId;
	string mMacAddress;
	string mIpAddress;
	char mFlag;
	int long mLeaseTime;
	const Settings& mSettings;
	databaseConnection mDbConnection;
};

#endif /* IPPOOLMAPPINGS_H_ */
