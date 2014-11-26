/*
 * sasaPackets.h
 *
 *  Created on: Nov 7, 2014
 *      Author: aditi
 */

#ifndef SASAPACKETS_H_
#define SASAPACKETS_H_
#include "IPPoolServerConstants.h"

// Request Packet
struct requestPacket{
	char mProtocolType;
	char mOpField;
	unsigned short int mServerId;
	unsigned int mChecksum;
	char mSrcHwAddress[HWD_LENGTH];
	unsigned int mRequestId;
	unsigned int mRequestedIp;
};

// Response Packet
struct responsePacket{
	char mProtocolType;
	char mOpField;
	unsigned short int mServerId;
	unsigned int mChecksum;
	char mSrcHwAddress[HWD_LENGTH];
	unsigned int mRequestId;
	unsigned int mAllocationValidTime;
	unsigned int mAllocatedIp;
	unsigned int mGatewayIp;
	unsigned int mSubnetMask;
	unsigned int mDnsIp;
};


#endif /* SASAPACKETS_H_ */
