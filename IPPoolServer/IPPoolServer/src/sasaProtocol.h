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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace log4cxx;
using namespace std;

class sasaProtocol{

public:
	sasaProtocol();

	void setRequestPacket(struct requestPacket lReqPack);
	struct responsePacket getResponsePacket();
	int ipRequestProcessing();


private:
	LoggerPtr mPLogger;
	struct requestPacket mReqPack;
	struct responsePacket mResPack;

	int copyReqToResFields();
	int ipOffer();
	int ipRequest();
	int ipRelease();
	int ipLeaseAcknowlegement();
	int otherConfigurationReq();
	int getOtherConfiguration();


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

int sasaProtocol::getOtherConfiguration(){
	if(mResPack != NULL){
		mResPack.mChecksum = 0;
		mResPack.mAllocationValidTime = 0;
		mResPack.mGatewayIp = 0;
		mResPack.mSubnetMask = 0;
		mResPack.mDnsIp = 0;
		LOG4CXX_INFO(mPLogger,"Other configurations set..");
	}
	else{
		LOG4CXX_ERROR(mPLogger,"Response packet null..other configurations cannot be set");
		return -1;
	}
	return 0;
}

int sasaProtocol::ipOffer(){

	if(copyReqToResFields() == 0){

		IpPoolMappings lIpPoolMapping;
		IPPool lIpPool;
		string lIpAddr;
		
		LOG4CXX_INFO(mPLogger,"Creating a new ip-mac mapping..");

		if(!(lIpPool.GetNextFreeIP(lIpAddr))){
			// getting free ip from the pool
			LOG4CXX_INFO(mPLogger,"Next free ip received from pool is "<< lIpAddr);
			in_addr lINAddr;
			inet_aton(lIpAddr.c_str(),&lINAddr);
			mResPack.mAllocatedIp = lINAddr.s_addr;

			// insert the entry of ip-mac into mapping table
			if(lIpPoolMapping.insertMapping(mResPack.mSrcHwAddress, lIpAddr)){
				LOG4CXX_ERROR(mPLogger,"IP-MAC mapping failed to be created..");
				return -1;
			}
			else{
				LOG4CXX_INFO(mPLogger,"IP-MAC mapping successfully created..");
			}

			mResPack.mOpField = '2';
			getOtherConfiguration();

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

	if(copyReqToResFields() == 0){

			IpPoolMappings lIpPoolMapping;
			IPPool lIpPool;
			string lIpAddr;
			in_addr lInAddr;
			lInAddr.s_addr = mReqPack.mRequestedIp;

			LOG4CXX_INFO(mPLogger,"Checking ip-mac mapping in the pool for requested Ip "<< mReqPack.mRequestedIp <<" and MAC "<< mReqPack.mSrcHwAddress);

			if(lIpPoolMapping.checkValidity(mReqPack.mSrcHwAddress,inet_ntoa(lInAddr))){
				LOG4CXX_INFO(mPLogger,"Mapping not found thus offering a new IP from the pool");
				ipOffer();
			}
			else{
				LOG4CXX_INFO(mPLogger,"Mapping found..");
				mResPack.mAllocatedIp = mReqPack.mRequestedIp;
				mResPack.mOpField = '4';
				getOtherConfiguration();
				ipLeaseAcknowlegement();
				LOG4CXX_INFO(mPLogger,"Acknowledging Ip Assignment "<< inet_ntoa(lInAddr)<<" to client with MAC "<< mReqPack.mSrcHwAddress);
			}
		}

		return 0;
}

int sasaProtocol::ipRelease(){
	IpPoolMappings lIpPoolMapping;
	IPPool lIpPool;

	if(mReqPack != NULL){
		in_addr lInAddr;
		lInAddr.s_addr = mReqPack.mRequestedIp;
		if(lIpPoolMapping.deleteMapping(mReqPack.mSrcHwAddress, inet_ntoa(lInAddr))){
			LOG4CXX_ERROR(mPLogger,"IP-MAC mapping could not be deleted");
			return -1;
		}
		else{
			LOG4CXX_INFO(mPLogger,"IP-MAC mapping deleted from the mapping pool...not releasing the ip address "<< inet_ntoa(lInAddr));
			if(lIpPool.ReleaseIP(inet_ntoa(lInAddr))){
				LOG4CXX_ERROR(mPLogger,"Could not release IP address "<< inet_ntoa(lInAddr));
				return -1;
			}
			else{
				LOG4CXX_INFO(mPLogger,"IP address "<< inet_ntoa(lInAddr)<< "released successfully..");
			}
		}
	}
	mResPack = NULL;
	return 0;
}

int sasaProtocol::ipLeaseAcknowlegement(){
	IpPoolMappings lIpPoolMapping;

	if(mReqPack != NULL){
		if((mReqPack.mSrcHwAddress != NULL) && (mReqPack.mRequestedIp != 0)){
			in_addr lInAddr;
			lInAddr.s_addr = mReqPack.mRequestedIp;
			if(lIpPoolMapping.setBindingFlag(mReqPack.mSrcHwAddress, inet_ntoa(lInAddr))){
				LOG4CXX_ERROR(mPLogger,"Binding failed corresponding to IP "<< inet_ntoa(lInAddr)<<" and MAC "<< mReqPack.mSrcHwAddress);
				return -1;
			}
			else{
				LOG4CXX_INFO(mPLogger,"Binding successful corresponding to IP "<< inet_ntoa(lInAddr)<<" and MAC "<< mReqPack.mSrcHwAddress);

			}
		}
	}
	return 0;
}

int sasaProtocol::otherConfigurationReq(){
	if(copyReqToResFields() == 0){
		in_addr lInAddr;
		lInAddr.s_addr = mReqPack.mRequestedIp;
		mResPack.mOpField = '5';
		mResPack.mRequestId = mReqPack.mRequestedIp;
		getOtherConfiguration();
		LOG4CXX_INFO(mPLogger,"Configurations gathered for IP "<< inet_ntoa(lInAddr));
	}
	else{
		return -1;
	}
	return 0;
}

void sasaProtocol::setRequestPacket(struct requestPacket lRespack){
	this->mReqPack = lRespack;
}

struct responsePacket sasaProtocol::getResponsePacket(){
	return this->mResPack;
}

int sasaProtocol::ipRequestProcessing(){
	char lOpcode;

	if(mReqPack != NULL){
		lOpcode = mReqPack.mOpField;

		switch(lOpcode){
		case '1':
			// Request for new / old Ip
			if(mReqPack.mRequestedIp == 0)
				ipOffer();
			else
				ipRequest();
			break;
		case '2':
			// opcode used for new IP Lease offer
			break;
		case '3':
			// release ip request received
			ipRelease();
			break;
		case '4':
			// opcode used for acknowledgment
			break;
		case '5':
			otherConfigurationReq();
			break;
		}
	}
	else{
		LOG4CXX_ERROR(mPLogger,"Request packet not obtained..");
	}
	return 0;
}



#endif /* SASAPROTOCOL_H_ */
