/*
 * TransactionIDMapper.h
 *
 *  Created on: Dec 1, 2014
 *      Author: surojit
 */

#ifndef TRANSACTIONIDMAPPER_H_
#define TRANSACTIONIDMAPPER_H_

#include <map>
#include "RequestPacketHolder.h"
#include <pthread.h>

class TransactionIDMapper {
private:
		std::map<unsigned int,RequestPacketHolder*> mMap;
		pthread_mutex_t lLock;
public:
	int AddRequestPacketHolder(RequestPacketHolder* pHolder);
	int RemovePacketHolder(unsigned int nRequestID);
	RequestPacketHolder* GetPacketForTransactionID(unsigned int nRequestID);
	TransactionIDMapper();
	virtual ~TransactionIDMapper();
};

#endif /* TRANSACTIONIDMAPPER_H_ */
