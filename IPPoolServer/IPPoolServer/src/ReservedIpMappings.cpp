/*
 * ReservedIpMappings.cpp
 *
 *  Created on: Dec 2, 2014
 *      Author: aditi
 */
#include "ReservedIpMappings.h"

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

int ReservedIpMappings::checkIfReservedIp(string pIpAddress){
	ostringstream lStrStream;
	int lResult;
	StoreQueryResult lStoreQueryResult;

	lStrStream.str("");
	lStrStream << "SELECT * FROM ip_reserved WHERE ip_address= '"<< pIpAddress<< "';";
	mDatabaseConnection.setMQuery(lStrStream.str());
	lResult = mDatabaseConnection.fireQuery();

	if(lResult < 0){
		LOG4CXX_ERROR(mPLogger,"Failed to check whether IP= "<< pIpAddress<<" is reserved or not");
		return -1;
	}

	lStoreQueryResult = mDatabaseConnection.getMResult();

	if(lStoreQueryResult.num_rows()>0){
		LOG4CXX_INFO(mPLogger,"IP= "<< pIpAddress<<" is a reserved ip..try to get another ip");
		return 0;
	}
	else{
		LOG4CXX_INFO(mPLogger,"IP= "<< pIpAddress<<" is not reserved..");
		return 1;
	}
}



