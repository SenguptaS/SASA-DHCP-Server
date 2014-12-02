/*
 * RequestPacketHolder.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: surojit
 */

#include "RequestPacketHolder.h"

RequestPacketHolder::RequestPacketHolder(unsigned int nTransactionID,std::string nClientRequestedIP) {
	// TODO Auto-generated constructor stub
	this->lTransactionID = nTransactionID;
	this->lClientRequestedIP = std::string(nClientRequestedIP);
}

RequestPacketHolder::~RequestPacketHolder() {
	// TODO Auto-generated destructor stub
}

