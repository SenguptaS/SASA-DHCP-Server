/*
 * ipPoolMappings.cpp
 *
 *  Created on: Nov 20, 2014
 *      Author: aditi
 */
#include "log4cxx/logger.h"
#include "IpPoolMappings.h"
#include "IPPoolServerConstants.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

IpPoolMappings::IpPoolMappings(const Settings& lSettings)
:mDbConnection(mSettings),mSettings(lSettings)
{
	mPLogger = Logger::getLogger(ROOT_LOGGER);
}

int IpPoolMappings::insertMapping(string lMacAddress, string lIpAddress) {

	in_addr lInAddr;
	lInAddr.s_addr = inet_addr(lIpAddress.c_str());

	ostringstream lStrStream;
	lStrStream.str("");

	lStrStream << "INSERT INTO ip_mapping (mac_address, ip_address) VALUES (" << inet_ntoa(lInAddr) <<", "<< lMacAddress <<");";

	mDbConnection.setMQuery(lStrStream.str());
	int lResult = mDbConnection.fireQuery();

	if( lResult < 0)
	{
		LOG4CXX_ERROR(mPLogger,"Failed to insert IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress);
		return -1;
	}

	LOG4CXX_INFO(mPLogger,"Successfully inserted IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress);
	return 0;
}

int IpPoolMappings::deleteMapping(string lMacAddress, string lIpAddress) {

	in_addr lInAddr;
	lInAddr.s_addr = inet_addr(lIpAddress.c_str());

	ostringstream lStrStream;
	lStrStream.str("");

	lStrStream << "DELETE FROM ip_mapping WHERE mac_address='" << lMacAddress <<"' AND ip_address='"<< inet_ntoa(lInAddr) <<"';";

	mDbConnection.setMQuery(lStrStream.str());
	int lResult = mDbConnection.fireQuery();

	if( lResult < 0)
	{
		LOG4CXX_ERROR(mPLogger,"Failed to delete IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress);
		return -1;
	}

	LOG4CXX_INFO(mPLogger,"Successfully deleted IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress);
	return 0;
}

int IpPoolMappings::setBindingFlag(string lMacAddress, string lIpAddress) {


	in_addr lInAddr;
	lInAddr.s_addr = inet_addr(lIpAddress.c_str());

	ostringstream lStrStream;
	lStrStream.str("");

	lStrStream << "UPDATE ip_mapping SET flag='B' WHERE mac_address='" << lMacAddress <<"' AND ip_address='"<< inet_ntoa(lInAddr) <<"';";

	mDbConnection.setMQuery(lStrStream.str());
	int lResult = mDbConnection.fireQuery();

	if( lResult < 0)
	{
		LOG4CXX_ERROR(mPLogger,"Failed to update the binding flag corresponding to IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress);
		return -1;
	}

	LOG4CXX_INFO(mPLogger,"Successfully updated binding flag corresponding to IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress);
	return 0;
}

int IpPoolMappings::updateLease() {
	return 0;
}

int IpPoolMappings::deleteMappingAsLeaseExpires() {
	return 0;
}

bool IpPoolMappings::checkValidity(string lMacAddress, string lIpAddress) {

	in_addr lInAddr;
	lInAddr.s_addr = inet_addr(lIpAddress.c_str());

	ostringstream lStrStream;
	lStrStream.str("");

	lStrStream << "SELECT COUNT(*) FROM ip_mapping WHERE mac_address='" << lMacAddress <<"' AND ip_address='"<< inet_ntoa(lInAddr) <<"';";

	mDbConnection.setMQuery(lStrStream.str());
	int lResult = mDbConnection.fireQuery();

	if( lResult < 0)
	{
		LOG4CXX_ERROR(mPLogger,"Failed to check validity of IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress);
		return false;
	}

	StoreQueryResult lQueryResult;
	lQueryResult = mDbConnection.getMResult();

	StoreQueryResult::const_iterator it;
	it = lQueryResult.begin();

	Row row = *it;
	if(row[0] == 1){
		LOG4CXX_INFO(mPLogger,"IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress <<" is valid and existing entry");
		return true;
	}
	else if (row[0] == 0){
		LOG4CXX_ERROR(mPLogger,"IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress <<" does not exist");
		return false;
	}
	else{
		LOG4CXX_ERROR(mPLogger,"IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress <<" exist multiple times");
		return false;
	}
}



