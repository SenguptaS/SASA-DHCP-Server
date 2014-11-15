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
#include "IPPoolServerConstants.h"

using namespace std;

class IpPoolMappings{
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

IpPoolMappings::IpPoolMappings(){
	mPLogger = Logger::getLogger(ROOT_LOGGER);
}

int IpPoolMappings::insertMapping(string lMacAddress, string lIpAddress){

	String lQuery = "insert into ip_mapping ()";
	return 0;
}

int IpPoolMappings::deleteMapping(string lMacAddress, string lIpAddress){

	return 0;
}

int IpPoolMappings::setBindingFlag(){

	return 0;
}

int IpPoolMappings::updateLease(){
 return 0;
}

int IpPoolMappings::deleteMappingAsLeaseExpires(){
	return 0;
}

bool IpPoolMappings::checkValidity(string lMacAddress, string lIpAddress){

	return false;
}




#endif /* IPPOOLMAPPINGS_H_ */
