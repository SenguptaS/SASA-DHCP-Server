/*
 * TransactionIDMapper.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: surojit
 */

#include "TransactionIDMapper.h"

TransactionIDMapper::TransactionIDMapper() {
	// TODO Auto-generated constructor stub
	pthread_mutexattr_t lMutexAttr;
	pthread_mutexattr_init(&lMutexAttr);
	pthread_mutex_init(&this->lLock, &lMutexAttr);
}

TransactionIDMapper::~TransactionIDMapper() {
	pthread_mutex_destroy(&this->lLock);
}

int TransactionIDMapper::AddRequestPacketHolder(RequestPacketHolder* pHolder) {
	pthread_mutex_lock(&lLock);
	this->mMap.insert(std::make_pair(pHolder->getTransactionId(), pHolder));
	pthread_mutex_unlock(&lLock);
	return 1;
}

int TransactionIDMapper::RemovePacketHolder(unsigned int nRequestID) {
	pthread_mutex_lock(&lLock);
	this->mMap.erase(nRequestID);
	pthread_mutex_unlock(&lLock);
	return 1;
}

RequestPacketHolder* TransactionIDMapper::GetPacketForTransactionID(
		unsigned int nRequestID) {

	RequestPacketHolder *pPacket = 0x0;

	pthread_mutex_lock(&lLock);
	std::map<unsigned int,RequestPacketHolder*>::iterator i =  this->mMap.find(nRequestID);
	pthread_mutex_unlock(&lLock);
	if(i == this->mMap.end())
	{
		return 0x00;
	}
	return pPacket;
}
