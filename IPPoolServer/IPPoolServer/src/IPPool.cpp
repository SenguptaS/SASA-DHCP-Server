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

IPPool::IPPool() {

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
	while (nNumberOfIPS > 0) {
		lOss.str("");
		lOss << "INSERT INTO `ip_pool` (`ip_address`,`flag`) VALUES('"
				<< inet_ntoa(lAddr) << "','N');";
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

}

int IPPool::ReleaseIP(std::string lIPAddress) {

}
