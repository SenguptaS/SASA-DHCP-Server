/*
 * RequestPacketHolder.h
 *
 *  Created on: Dec 1, 2014
 *      Author: surojit
 */

#ifndef REQUESTPACKETHOLDER_H_
#define REQUESTPACKETHOLDER_H_
#include <string>
class RequestPacketHolder {
private:
	std::string lClientRequestedIP;
	unsigned int lTransactionID;

public:
	RequestPacketHolder(unsigned int nTransactionID,std::string nClientRequestedIP);
	virtual ~RequestPacketHolder();

	unsigned int getTransactionId() const {
		return lTransactionID;
	}

	const std::string& getClientRequestedIp() const {
		return lClientRequestedIP;
	}
};

#endif /* REQUESTPACKETHOLDER_H_ */
