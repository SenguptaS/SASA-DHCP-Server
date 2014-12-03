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
#include "iostream"
using namespace std;

IpPoolMappings::IpPoolMappings(const Settings& lSettings) :
		mDbConnection(mSettings), mSettings(lSettings) {
	mPLogger = Logger::getLogger(ROOT_LOGGER);
}

int IpPoolMappings::insertMapping(string lMacAddress, string lIpAddress) {

	in_addr lInAddr;
	lInAddr.s_addr = inet_addr(lIpAddress.c_str());

	ostringstream lStrStream;
	lStrStream.str("");

	lStrStream << "INSERT INTO ip_mapping (mac_address, ip_address, lease_time) VALUES ('" << lMacAddress
			<< "', '" << inet_ntoa(lInAddr) << "', "<< (mSettings.mLeaseTime + time(NULL)) << ");";

	mDbConnection.setMQuery(lStrStream.str());
	int lResult = mDbConnection.fireQuery();

	if (lResult < 0) {
		LOG4CXX_ERROR(mPLogger,
				"Failed to insert IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<  lMacAddress);
		return -1;
	}

	LOG4CXX_INFO(mPLogger,
			"Successfully inserted IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress);
	return 0;
}

int IpPoolMappings::deleteMapping(string lMacAddress, string lIpAddress) {

	in_addr lInAddr;
	lInAddr.s_addr = inet_addr(lIpAddress.c_str());

	ostringstream lStrStream;
	lStrStream.str("");

	lStrStream << "DELETE FROM ip_mapping WHERE mac_address='" << lMacAddress
			<< "' AND ip_address='" << inet_ntoa(lInAddr) << "';";

	mDbConnection.setMQuery(lStrStream.str());
	int lResult = mDbConnection.fireQuery();

	if (lResult < 0) {
		LOG4CXX_ERROR(mPLogger,
				"Failed to delete IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress);
		return -1;
	}

	LOG4CXX_INFO(mPLogger,
			"Successfully deleted IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress);
	return 0;
}

int IpPoolMappings::setBindingFlag(string lMacAddress, string lIpAddress) {

	in_addr lInAddr;
	lInAddr.s_addr = inet_addr(lIpAddress.c_str());

	ostringstream lStrStream;
	lStrStream.str("");

	lStrStream << "UPDATE ip_mapping SET flag='B' WHERE mac_address='"
			<< lMacAddress << "' AND ip_address='" << inet_ntoa(lInAddr)
			<< "';";

	mDbConnection.setMQuery(lStrStream.str());
	int lResult = mDbConnection.fireQuery();

	if (lResult < 0) {
		LOG4CXX_ERROR(mPLogger,
				"Failed to update the binding flag corresponding to IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress);
		return -1;
	}

	LOG4CXX_INFO(mPLogger,
			"Successfully updated binding flag corresponding to IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress);

	lStrStream.str("");
	lStrStream <<"UPDATE ip_pool SET flag='N' WHERE ip_address IN (SELECT ip_address from ip_mapping "
			"WHERE flag='P' AND mac_address= '"<< lMacAddress<<"');";
	mDbConnection.setMQuery(lStrStream.str());
	lResult = mDbConnection.fireQuery();

	if(lResult < 0){
			LOG4CXX_ERROR(mPLogger,"Failed to free all other IP addresses offered to client with MAC= "<< lMacAddress);
			return -1;
	}

	lStrStream.str("");
	lStrStream << "DELETE FROM ip_mapping WHERE mac_address='"
			<< lMacAddress << "' AND flag='P';";

	mDbConnection.setMQuery(lStrStream.str());
	lResult = mDbConnection.fireQuery();

	if(lResult < 0){
		LOG4CXX_ERROR(mPLogger,"Failed to clean all other mapping entries corresponding to MAC "<< lMacAddress);
		return -1;
	}

	LOG4CXX_INFO(mPLogger,"Successfully released all other ip addresses offered to the client with MAC= "<< lMacAddress);
	return 0;
}

int IpPoolMappings::updateLease(string lMacAddress, string lIpAddress) {

	unsigned int lcurrentTime;
	int lResult;
	ostringstream lStrStream;
	in_addr lInAddr;

	lInAddr.s_addr = inet_addr(lIpAddress.c_str());
	lStrStream.str("");
	lcurrentTime = time(NULL);

	lStrStream << "UPDATE ip_mapping SET lease_time= "<< (lcurrentTime + mSettings.mLeaseTime) <<" WHERE mac_address ='" << lMacAddress << "' AND ip_address= '"<< lIpAddress << "';";
	mDbConnection.setMQuery(lStrStream.str());
	lResult = mDbConnection.fireQuery();

	if(lResult <0){
		LOG4CXX_ERROR(mPLogger, "Failed to update the lease time of IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress);
		return -1;
	}

	LOG4CXX_INFO(mPLogger,"Successfully updated the lease time of IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress);
	return 0;
}

int IpPoolMappings::deleteMappingAsLeaseExpires() {

	unsigned int lcurrentTime;
	int lResult;
	ostringstream lStrstream;
	lStrstream.str("");

	lcurrentTime = time(NULL);

	lStrstream << "UPDATE ip_pool SET flag='N' WHERE ip_address IN (SELECT ip_address FROM ip_mapping WHERE lease_time < " << lcurrentTime << ");";

	mDbConnection.setMQuery(lStrstream.str());
	lResult = mDbConnection.fireQuery();

	if(lResult < 0){
		LOG4CXX_ERROR(mPLogger, "Failed to free IP addresses corresponding to the expired lease assignments");
		return -1;
	}

	lStrstream.str("");

	lStrstream << "DELETE FROM ip_mapping WHERE lease_time < " << lcurrentTime <<";";
	mDbConnection.setMQuery(lStrstream.str());
	lResult = mDbConnection.fireQuery();

	if(lResult < 0){
		LOG4CXX_ERROR(mPLogger, "Failed to delete mapping corresponding to the expired leases");
		return -1;
	}

	return 0;
}

bool IpPoolMappings::checkValidity(string lMacAddress, string lIpAddress) {

	in_addr lInAddr;
	lInAddr.s_addr = inet_addr(lIpAddress.c_str());

	ostringstream lStrStream;
	lStrStream.str("");

	lStrStream << "SELECT COUNT(*) FROM ip_mapping WHERE mac_address='"
			<< lMacAddress << "' AND ip_address='" << inet_ntoa(lInAddr)
			<< "';";

	mDbConnection.setMQuery(lStrStream.str());
	int lResult = mDbConnection.fireQuery();

	if (lResult < 0) {
		LOG4CXX_ERROR(mPLogger,
				"Failed to check validity of IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress);
		return false;
	}

	StoreQueryResult lQueryResult;
	lQueryResult = mDbConnection.getMResult();

	if ( ((int)lQueryResult[0][0]) == 1) {
		LOG4CXX_INFO(mPLogger,
				"IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress <<" is valid and existing entry");
		return true;
	} else if ( ((int)lQueryResult[0][0])  == 0) {
		LOG4CXX_ERROR(mPLogger,
				"IP-MAC mapping with IP= "<< inet_ntoa(lInAddr)<<" and MAC= "<<lMacAddress <<" does not exist");
		return false;
	}
}

