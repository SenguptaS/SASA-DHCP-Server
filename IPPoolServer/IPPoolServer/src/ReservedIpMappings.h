/*
 * ReservedIpMappings.h
 *
 *  Created on: Dec 1, 2014
 *      Author: aditi
 */

#ifndef RESERVEDIPMAPPINGS_H_
#define RESERVEDIPMAPPINGS_H_
#include "Settings.h"
#include "databaseConnection.h"
#include "IPPoolServerConstants.h"
#include <log4cxx/logger.h>
#include <string>
#include "IPPool.h"


class ReservedIpMappings{

public:
	ReservedIpMappings(const Settings &lSettings);
	bool isReservedIpAvailable(string pMacAddress);
	int getReservedIp(string pMacAddress, string &pIpAddress);

private:
	const Settings& mSettings;
	databaseConnection mDatabaseConnection;
	log4cxx::LoggerPtr mPLogger;
};


ReservedIpMappings::ReservedIpMappings(const Settings &lSettings):
		mDatabaseConnection(lSettings), mSettings(lSettings)
{
	mPLogger = log4cxx::Logger::getLogger(ROOT_LOGGER);
}

bool ReservedIpMappings::isReservedIpAvailable(string pMacAddress){
	ostringstream lStrStream;
	int lResult;
	StoreQueryResult lStoreRes;

	lStrStream.str("");

	lStrStream << "SELECT * FROM ip_reserved WHERE client_host='"<< pMacAddress <<"';";

	mDatabaseConnection.setMQuery(lStrStream.str());
	lResult = mDatabaseConnection.fireQuery();

	if(lResult<0){
		LOG4CXX_ERROR(mPLogger,"Failed to check for reserved Ip for client with MAC= "<< pMacAddress);
		return false;
	}

	lStoreRes = mDatabaseConnection.getMResult();

	if(lStoreRes.num_rows()>0){
		LOG4CXX_INFO(mPLogger,"Reserved ip available for the client with MAC= "<< pMacAddress);
		return true;
	}
	else{
		LOG4CXX_ERROR(mPLogger,"Reserved ip not available for the client with MAC= "<< pMacAddress);
		return false;
	}
}

int ReservedIpMappings::getReservedIp(string pMacAddress, string &pIpAddress){
	ostringstream lStrStream;
	int lResult;
	IPPool lIpPool(mSettings);
	StoreQueryResult lStoreQueryResult;

	lStrStream.str("");
	lStrStream << "SELECT ip_address FROM ip_reserved WHERE client_host='"<< pMacAddress<< "';";
	mDatabaseConnection.setMQuery(lStrStream.str());
	lResult = mDatabaseConnection.fireQuery();

	if(lResult<0){
		LOG4CXX_ERROR(mPLogger,"Failed to retrieve reserved ip for client with MAC= "<< pMacAddress);
		return -1;
	}

	lStoreQueryResult = mDatabaseConnection.getMResult();
	pIpAddress = (string)lStoreQueryResult[0][0];

	if(pIpAddress.empty()){
		LOG4CXX_ERROR(mPLogger,"Failed to retrieve reserved ip for client with MAC= "<< pMacAddress);
		return -1;
	}
	else{
		LOG4CXX_INFO(mPLogger,"Reserved ip for client with MAC= "<< pMacAddress<<" is "<< pIpAddress);
		if (lIpPool.SetIPAddressState(pIpAddress, true) < 0) {
			pIpAddress = "";
			return -1;
		}
		return 0;
	}
}

#endif /* RESERVEDIPMAPPINGS_H_ */
