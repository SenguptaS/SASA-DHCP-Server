/*
 * IPPool.cpp
 *
 *  Created on: Nov 14, 2014
 *      Author: surojit
 */

#include "IPPool.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <mysql++/connection.h>
#include "IPPoolServerConstants.h"
#include "ReservedIpMappings.h"

IPPool::IPPool(const Settings& lSettings) :
		mDatabaseConnection(lSettings), mSettings(lSettings) {
	this->InitializePool();

}

IPPool::~IPPool() {
	// TODO Auto-generated destructor stub
}

int IPPool::InitializePool() {
	pLogger = log4cxx::Logger::getLogger(ROOT_LOGGER);
	return 0;
}

int IPPool::AddIPRange(std::string lStartingAddress,
		unsigned int nNumberOfIPS) {

	in_addr lAddr;

	in_addr_t lAddress = inet_addr(lStartingAddress.c_str());
	std::ostringstream lOss;
	unsigned int lNoIPAdded = 0;

	lAddr.s_addr = lAddress;

	while (nNumberOfIPS > 0) {
		lOss.str("");
		lOss << "INSERT INTO `ip_pool` (`ip_address`,`flag`) VALUES('"
				<< inet_ntoa(lAddr) << "','N');";

		lAddress = htonl( lAddr.s_addr );
		lAddress++;
		lAddr.s_addr = ntohl(lAddress);

		mDatabaseConnection.setMQuery(lOss.str());
		int lQueryReturnValue = mDatabaseConnection.fireQuery();
		if (lQueryReturnValue < 0) {
			LOG4CXX_ERROR(pLogger,
					"Failed to insert IP: " << inet_ntoa(lAddr) << " into pool table");
			nNumberOfIPS--;
			continue;
		}
		LOG4CXX_DEBUG(pLogger,
				"Added IP: " << inet_ntoa(lAddr) << " to database.");
		lNoIPAdded++;
		nNumberOfIPS--;
	}
	return lNoIPAdded;
}

int IPPool::SetHoles(std::string lStartingAddress, unsigned int nNumberOfIPS) {
	in_addr lAddr;
	in_addr_t lAddress = inet_addr(lStartingAddress.c_str());
	std::ostringstream lOss;
	unsigned int lNoIPDeleted = 0;
	while (nNumberOfIPS > 0) {
		lAddr.s_addr = lAddress;
		lOss.str("");
		lOss << "DELETE `ip_pool` WHERE `ip_address`='" << inet_ntoa(lAddr)
				<< "'";
		mDatabaseConnection.setMQuery(lOss.str());
		int lQueryReturnValue = mDatabaseConnection.fireQuery();
		if (lQueryReturnValue < 0) {
			LOG4CXX_ERROR(pLogger,
					"Failed to set hole for IP: " << inet_ntoa( lAddr) << " in pool table");
			nNumberOfIPS--;
			continue;
		}
		LOG4CXX_DEBUG(pLogger,
				"Set hole for IP: " << inet_ntoa( lAddr) << " in pool table.");
		lNoIPDeleted++;
		nNumberOfIPS--;
	}
	return lNoIPDeleted;
}

int IPPool::SetAvailable(std::string lStartingAddress,
		unsigned int nNumberOfIPS) {
	in_addr lAddr;
	in_addr_t lAddress = inet_addr(lStartingAddress.c_str());
	std::ostringstream lOss;
	unsigned int lNoIPMarked = 0;
	while (nNumberOfIPS > 0) {
		lAddr.s_addr = lAddress;
		lOss.str("");
		lOss << inet_ntoa(lAddr);
		if (this->AddIPRange(lOss.str(), 1)) {

			lOss.str("");
			lOss << "UPDATE `ip_pool` SET `flag` = 'N' WHERE `ip_address`='"
					<< inet_ntoa(lAddr) << "'";
			mDatabaseConnection.setMQuery(lOss.str());
			int lQueryReturnValue = mDatabaseConnection.fireQuery();
			if (lQueryReturnValue < 0) {
				LOG4CXX_ERROR(pLogger,
						"Failed to set IP: " << inet_ntoa(lAddr) << " as available");
				nNumberOfIPS--;
				continue;
			}
			LOG4CXX_DEBUG(pLogger,
					"Set IP: " << inet_ntoa(lAddr) << " as available.");
			lNoIPMarked++;
			nNumberOfIPS--;
		}
	}
	return lNoIPMarked;
}

int IPPool::GetNextFreeIP(std::string& lNextFreeIP) {
	std::ostringstream lOss;
	ReservedIpMappings lReservedIpMappings(mSettings);

	do{
		lOss << "SELECT * FROM `ip_pool` WHERE `flag`='N' LIMIT 1";
		mDatabaseConnection.setMQuery(lOss.str());

		if (mDatabaseConnection.fireQuery() < 0) {
			LOG4CXX_ERROR(pLogger, "Failed to get free ip from pool");
			return -1;
		}

		StoreQueryResult lResult = mDatabaseConnection.getMResult();
		if (lResult == NULL || lResult.num_rows() <= 0) {
			LOG4CXX_ERROR(pLogger, "No free ip address available in pool");
			return -1;
		}

		lOss.str("");
		lOss << (string) lResult[0]["ip_address"];
		lNextFreeIP = lOss.str();
	} while(lReservedIpMappings.checkIfReservedIp(lNextFreeIP) != 1);

	if (this->SetIPAddressState(lNextFreeIP, true) < 0) {
		lNextFreeIP = "";
		return -1;
	}
	return 0;
}

int IPPool::ReleaseIP(std::string lIPAddress) {
	return this->SetIPAddressState(lIPAddress, false);
}

int IPPool::SetIPAddressState(std::string lIPAddress, bool lInUse) {
	std::ostringstream lOss;
	lOss << "UPDATE `ip_pool` SET `flag` = '" << (lInUse ? "A" : "N") << "' WHERE ip_address = '" << lIPAddress <<"'";

	mDatabaseConnection.setMQuery(lOss.str());

	if (mDatabaseConnection.fireQuery() < 0) {
		LOG4CXX_ERROR(pLogger,
				"Failed to update status of IP: " << lIPAddress << " to " << (lInUse?"A":"N"));
		return -1;
	}
	LOG4CXX_INFO(pLogger,
			"Set status of IP Address: " << lIPAddress << " to " << (lInUse?"A":"N"));
	return 0;
}

bool IPPool::isIpAvailable(std::string lIPAddress){
	std::ostringstream lOss;
	StoreQueryResult lStoreRes;
	lOss << "SELECT * FROM ip_pool WHERE ip_address= '"<< lIPAddress << "' AND flag='N';";

	mDatabaseConnection.setMQuery(lOss.str());

	if(mDatabaseConnection.fireQuery() < 0){
		LOG4CXX_ERROR(pLogger,
						"Failed to check current status of IP "<< lIPAddress);
		return false;
	}

	lStoreRes = mDatabaseConnection.getMResult();
	if(lStoreRes.num_rows()>0){
		LOG4CXX_INFO(pLogger,"Requested Ip "<< lIPAddress<<" is available..");

		if (this->SetIPAddressState(lIPAddress, true) < 0) {
			lIPAddress = "";
			return false;
		}

		return true;
	}
	else{
		LOG4CXX_INFO(pLogger,"Requested ip "<< lIPAddress<<" is unavailable..");
		return false;
	}
}

