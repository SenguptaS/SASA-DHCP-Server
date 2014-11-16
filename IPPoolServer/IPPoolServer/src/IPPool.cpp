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

int IPPool::AddIPRange(std::string lStartingAddress, int nNumberOfIPS) {
	 in_addr_t lAddress = inet_addr(lStartingAddress.c_str());
	 std::ostringstream lOss;

	 while(nNumberOfIPS > 0)
	 {
		 lOss.str("");
		 lOss << "INSERT INTO `ip_pool` (`ip_address`,`flag`) VALUES('" << inet_ntoa((in_addr)lAddress) << "','N');";
//		 mDatabaseConnection.
		 int lQueryReturnValue = mDatabaseConnection.fireQuery();
		 if( lQueryReturnValue <0 )
		 {
			 LOG4CXX_ERROR(pLogger,"Failed to insert ip: " );
		 }
	 }
}

int IPPool::SetHoles(std::string lStartingAddress, int nNumberOfIPS) {

}

int IPPool::SetAvailable(std::string lStartingAddress, int nNumberOfIPS) {

}

int IPPool::GetNextFreeIP(std::string& lNextFreeIP) {

}

int IPPool::ReleaseIP(std::string lIPAddress) {

}
