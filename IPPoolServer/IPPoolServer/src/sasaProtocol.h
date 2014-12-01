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
#include "IPPoolServerConstants.h"
#include "Utility.h"
#include "ReservedIpMappings.h"

using namespace log4cxx;
using namespace std;

class sasaProtocol{

public:
	sasaProtocol(const Settings& lSettings);

	void setRequestPacket(requestPacket* lReqPack);
	responsePacket* getResponsePacket();
	int ipRequestProcessing();


private:
	LoggerPtr mPLogger;
	requestPacket* mReqPack;
	responsePacket* mResPack;
	const Settings& mSettings;

	int copyReqToResFields();
	int ipOffer();
	int ipRequest();
	int ipRelease();
	int ipLeaseAcknowlegement();
	int otherConfigurationReq();
	int getOtherConfiguration();
	int staticIpOffer();


};


sasaProtocol::sasaProtocol(const Settings& lSettings)
	: mSettings(lSettings){
	mPLogger = Logger::getLogger(ROOT_LOGGER);
}

int sasaProtocol::copyReqToResFields(){
	if(mReqPack == NULL){
		LOG4CXX_ERROR(mPLogger, "Request packet is null");
		return -1;
	}
	else{
		mResPack->mProtocolType = mReqPack->mProtocolType;
		mResPack->mServerId = mReqPack->mServerId;
		for(int i=0; i< HWD_LENGTH; i++)
			mResPack->mSrcHwAddress[i] = mReqPack->mSrcHwAddress[i];
		mResPack->mRequestId = mReqPack->mRequestId;
		LOG4CXX_INFO(mPLogger,"Copied required fields in response packet from request packet");
		return 0;
	}
}

int sasaProtocol::getOtherConfiguration(){
	if(mResPack != NULL){
		in_addr lInAddr;
		mResPack->mChecksum = 0;

		mResPack->mAllocationValidTime = mSettings.mLeaseTime;

		inet_aton(mSettings.mGatewayIp.c_str(), &lInAddr);
		mResPack->mGatewayIp = lInAddr.s_addr;

		inet_aton(mSettings.mSubnetMask.c_str(), &lInAddr);
		mResPack->mSubnetMask = lInAddr.s_addr;

		inet_aton(mSettings.mDnsIp.c_str(), &lInAddr);
		mResPack->mDnsIp = lInAddr.s_addr;

		LOG4CXX_INFO(mPLogger,"Other configurations set..");
	}
	else{
		LOG4CXX_ERROR(mPLogger,"Response packet null..other configurations cannot be set");
		return -1;
	}
	return 0;
}

int sasaProtocol::ipOffer(){


	IpPoolMappings lIpPoolMapping(mSettings);
	IPPool lIpPool(mSettings);
	ReservedIpMappings lReservedIpMappings(mSettings);
	string lIpAddr, lSrcHwdAddr;


	lIpPoolMapping.deleteMappingAsLeaseExpires();

	if(!lReservedIpMappings.isReservedIpAvailable(Utility::GetPrintableMac(this->mReqPack->mSrcHwAddress))){
		if(copyReqToResFields() == 0){

			lSrcHwdAddr = Utility::GetPrintableMac(this->mReqPack->mSrcHwAddress);

			LOG4CXX_INFO(mPLogger,"Creating a new ip-mac mapping");

			if(lIpPool.GetNextFreeIP(lIpAddr)==0){
				// getting free ip from the pool
				LOG4CXX_INFO(mPLogger,"Next free ip received from pool is "<< lIpAddr);
				in_addr lINAddr;
				inet_aton(lIpAddr.c_str(),&lINAddr);
				mResPack->mAllocatedIp = lINAddr.s_addr;

				// insert the entry of ip-mac into mapping table
				if(lIpPoolMapping.insertMapping(lSrcHwdAddr, lIpAddr)){
					LOG4CXX_ERROR(mPLogger,"IP-MAC mapping failed to be created..");
					return -1;
				}
				else{
					LOG4CXX_INFO(mPLogger,"IP-MAC mapping successfully created..");
				}

				mResPack->mOpField = 2;
				getOtherConfiguration();

				LOG4CXX_INFO(mPLogger,"IP lease offered successfully!!");

			}
			else{
				LOG4CXX_ERROR(mPLogger, "Failed to get free ip from pool");
				return -1;
			}
		}
	}
	else{
		staticIpOffer();
	}
	return 0;
}

int sasaProtocol::staticIpOffer(){
	IpPoolMappings lIpPoolMapping(mSettings);
	IPPool lIpPool(mSettings);
	ReservedIpMappings lReservedIpMappings(mSettings);
	string lIpAddr, lSrcHwdAddr;

	lSrcHwdAddr = Utility::GetPrintableMac(this->mReqPack->mSrcHwAddress);

	if(copyReqToResFields() == 0){

			LOG4CXX_INFO(mPLogger,"Creating a new ip-mac mapping");

			if(lReservedIpMappings.getReservedIp(lSrcHwdAddr, lIpAddr)==0){
				// getting free ip from the pool
				LOG4CXX_INFO(mPLogger,"Reserved ip for the client with MAC= "<< lSrcHwdAddr<<" is "<< lIpAddr);
				in_addr lINAddr;
				inet_aton(lIpAddr.c_str(),&lINAddr);
				mResPack->mAllocatedIp = lINAddr.s_addr;

				// insert the entry of ip-mac into mapping table
				if(lIpPoolMapping.insertMapping(lSrcHwdAddr, lIpAddr)){
					LOG4CXX_ERROR(mPLogger,"IP-MAC mapping failed to be created..");
					return -1;
				}
				else{
					LOG4CXX_INFO(mPLogger,"IP-MAC mapping successfully created..");
				}

				mResPack->mOpField = 2;
				getOtherConfiguration();

				LOG4CXX_INFO(mPLogger,"IP lease offered successfully!!");

			}
			else{
				LOG4CXX_ERROR(mPLogger,"Failed to retrieve reserved ip for client with MAC= "<< lSrcHwdAddr);
				return -1;
			}
	}
	return 0;
}

int sasaProtocol::ipRequest(){

	IpPoolMappings lIpPoolMapping(mSettings);
	IPPool lIpPool(mSettings);
	string lIpAddr, lSrcHwdaddr;
	in_addr lInAddr;

	lIpPoolMapping.deleteMappingAsLeaseExpires();

	if(copyReqToResFields() == 0){

			lInAddr.s_addr = mReqPack->mRequestedIp;
			lSrcHwdaddr = Utility::GetPrintableMac(mReqPack->mSrcHwAddress);

			LOG4CXX_INFO(mPLogger,"Checking ip-mac mapping in the pool for requested Ip "<< inet_ntoa(lInAddr) <<" and MAC "<< Utility::GetPrintableMac(mReqPack->mSrcHwAddress));

			if(!(lIpPoolMapping.checkValidity(lSrcHwdaddr,inet_ntoa(lInAddr)))){
				LOG4CXX_INFO(mPLogger,"Mapping not found thus offering a new IP from the pool");
				this->ipOffer();
			}
			else{
				LOG4CXX_INFO(mPLogger,"Mapping found..");
				lIpPoolMapping.updateLease(lSrcHwdaddr,inet_ntoa(lInAddr));
				mResPack->mAllocatedIp = mReqPack->mRequestedIp;
				mResPack->mOpField = 4;
				getOtherConfiguration();
				ipLeaseAcknowlegement();
				LOG4CXX_INFO(mPLogger,"Acknowledging Ip Assignment "<< inet_ntoa(lInAddr)<<" to client with MAC "<<  Utility::GetPrintableMac(mReqPack->mSrcHwAddress));
			}
		}

		return 0;
}

int sasaProtocol::ipRelease(){
	IpPoolMappings lIpPoolMapping(mSettings);
	IPPool lIpPool(mSettings);
	string lSrcHwdAddr;

	lSrcHwdAddr = Utility::GetPrintableMac(mReqPack->mSrcHwAddress);

	if(mReqPack != NULL){
		in_addr lInAddr;
		lInAddr.s_addr = mReqPack->mRequestedIp;
		if(lIpPoolMapping.deleteMapping(lSrcHwdAddr, inet_ntoa(lInAddr))){
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
	IpPoolMappings lIpPoolMapping(mSettings);

	if(mReqPack != NULL){
		if((mReqPack->mSrcHwAddress != NULL) && (mReqPack->mRequestedIp != 0)){
			in_addr lInAddr;
			string lSrcHwdAddr;

			lSrcHwdAddr = Utility::GetPrintableMac(mReqPack->mSrcHwAddress);

			lInAddr.s_addr = mReqPack->mRequestedIp;
			if(lIpPoolMapping.setBindingFlag(lSrcHwdAddr, inet_ntoa(lInAddr))){
				LOG4CXX_ERROR(mPLogger,"Binding failed corresponding to IP "<< inet_ntoa(lInAddr)<<" and MAC "<<  Utility::GetPrintableMac(mReqPack->mSrcHwAddress));
				return -1;
			}
			else{
				LOG4CXX_INFO(mPLogger,"Binding successful corresponding to IP "<< inet_ntoa(lInAddr)<<" and MAC "<<  Utility::GetPrintableMac(mReqPack->mSrcHwAddress));

			}
		}
	}
	return 0;
}

int sasaProtocol::otherConfigurationReq(){
	if(copyReqToResFields() == 0){
		in_addr lInAddr;
		lInAddr.s_addr = mReqPack->mRequestedIp;
		mResPack->mOpField = 6;
		mResPack->mAllocatedIp = mReqPack->mRequestedIp;
		getOtherConfiguration();
		LOG4CXX_INFO(mPLogger,"Configurations gathered for IP "<< inet_ntoa(lInAddr));
	}
	else{
		return -1;
	}
	return 0;
}

void sasaProtocol::setRequestPacket(requestPacket* lReqpack){
	this->mReqPack = lReqpack;
}

responsePacket* sasaProtocol::getResponsePacket(){
	return this->mResPack;
}

int sasaProtocol::ipRequestProcessing(){
	char lOpcode;

	if(mReqPack != NULL){
		lOpcode = mReqPack->mOpField;

		switch(lOpcode){
		case 1:
			// Request for new Ip
			ipOffer();
			break;
		case 2:
			// opcode used for new IP Lease offer
			break;
		case 3:
			// Request for old Ip
			ipRequest();
			break;
		case 4:
			// opcode used for acknowledgment
			break;
		case 5:
			// release ip request received
			ipRelease();
			break;
		case 6:
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
