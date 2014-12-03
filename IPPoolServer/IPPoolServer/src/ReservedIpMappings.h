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
	int checkIfReservedIp(string pIpAddress);

private:
	const Settings& mSettings;
	databaseConnection mDatabaseConnection;
	log4cxx::LoggerPtr mPLogger;
};

#endif /* RESERVEDIPMAPPINGS_H_ */
