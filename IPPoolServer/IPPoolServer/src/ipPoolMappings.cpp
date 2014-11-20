/*
 * ipPoolMappings.cpp
 *
 *  Created on: Nov 20, 2014
 *      Author: aditi
 */
#include "log4cxx/logger.h"
#include "IpPoolMappings.h"
#include "IPPoolServerConstants.h"

IpPoolMappings::IpPoolMappings() {
	mPLogger = Logger::getLogger(ROOT_LOGGER);
}

int IpPoolMappings::insertMapping(string lMacAddress, string lIpAddress) {

	String lQuery = "insert into ip_mapping ()";
	return 0;
}

int IpPoolMappings::deleteMapping(string lMacAddress, string lIpAddress) {

	return 0;
}

int IpPoolMappings::setBindingFlag() {

	return 0;
}

int IpPoolMappings::updateLease() {
	return 0;
}

int IpPoolMappings::deleteMappingAsLeaseExpires() {
	return 0;
}

bool IpPoolMappings::checkValidity(string lMacAddress, string lIpAddress) {

	return false;
}



