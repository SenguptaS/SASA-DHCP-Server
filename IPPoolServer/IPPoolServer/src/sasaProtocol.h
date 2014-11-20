/*
 * sasaProtocol.h
 *
 *  Created on: Nov 20, 2014
 *      Author: aditi
 */

#ifndef SASAPROTOCOL_H_
#define SASAPROTOCOL_H_
#include <log4cxx/logger.h>
#include <unistd.h>
#include "sasaPackets.h"
#include "IpPoolMappings.h"
#include "IPPool.h"
#include <string>
using namespace log4cxx;
using namespace std;

class sasaProtocol{

public:
	sasaProtocol();
	int leaseOffer();
	int ipRequest();
	int ipLeaseAcknowlegement();
	int ipRelease();
	int backgroundProcessing();
	void setRequestPacket(struct requestPacket lReqPack);
	struct responsePacket getResponsePacket();
	int copyReqToResFields();

private:
	LoggerPtr mPLogger;
	struct requestPacket mReqPack;
	struct responsePacket mResPack;

};

sasaProtocol::sasaProtocol(){
	mPLogger = Logger::getLogger(ROOT_LOGGER);
}

int sasaProtocol::copyReqToResFields(){
	if(mReqPack == NULL){
		LOG4CXX_ERROR(mPLogger, "Request packet is null");
		return -1;
	}
	else{
		mResPack.mProtocolType = mReqPack.mProtocolType;
		mResPack.mServerId = mReqPack.mServerId;
		mResPack.mSrcHwAddress = mReqPack.mSrcHwAddress;
		mResPack.mRequestId = mReqPack.mRequestId;
		LOG4CXX_INFO(mPLogger,"Copied required fields in response packet from request packet");
		return 0;
	}
}

int sasaProtocol::leaseOffer(){

	if(copyReqToResFields() == 0){

		IpPoolMappings lIpPoolMapping;
		IPPool lIpPool;
		string lIpAddr;

		LOG4CXX_INFO(mPLogger,"Creating a new ip-mac mapping..");

		if(!(lIpPool.GetNextFreeIP(lIpAddr))){
			// getting free ip from the pool
			LOG4CXX_INFO(mPLogger,"Next free ip received from pool is "<< lIpAddr);
			mResPack.mAllocatedIp = lIpAddr;

			// insert the entry of ip-mac into mapping table
			if(lIpPoolMapping.insertMapping(mResPack.mSrcHwAddress, lIpAddr)){
				LOG4CXX_ERROR(mPLogger,"IP-MAC mapping failed to be created..");
				return -1;
			}
			else{
				LOG4CXX_INFO(mPLogger,"IP-MAC mapping successfully created..");
			}

			mResPack.mChecksum = 0;
			mResPack.mAllocationValidTime = 0;
			mResPack.mGatewayIp = 0;
			mResPack.mSubnetMask = 0;
			mResPack.mDnsIp = 0;
			mResPack.mOpField = '2';

			LOG4CXX_INFO(mPLogger,"IP lease offered successfully!!");

		}
		else{
			LOG4CXX_ERROR(mPLogger, "Failed to get free ip from pool");
			return -1;
		}
	}

	return 0;
}

int sasaProtocol::ipRequest(){

	return 0;
}

int sasaProtocol::ipLeaseAcknowlegement(){

	return 0;
}

int sasaProtocol::ipRelease(){

	return 0;
}

int sasaProtocol::backgroundProcessing(){

	return 0;
}

void sasaProtocol::setRequestPacket(struct requestPacket lRespack){
	this->mReqPack = lRespack;
}

struct responsePacket sasaProtocol::getResponsePacket(){
	return this->mResPack;
}




#endif /* SASAPROTOCOL_H_ */
