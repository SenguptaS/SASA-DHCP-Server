/*
 * sasaPackets.h
 *
 *  Created on: Nov 7, 2014
 *      Author: aditi
 */

#ifndef SASAPACKETS_H_
#define SASAPACKETS_H_


// Request Packet
struct SASA_requestPacket{
	char mProtocolType;
	char mOpField;
	unsigned short int mServerId;
	unsigned int mChecksum;
	unsigned long int mSrcHwAddress;
	unsigned int mRequestId;
	unsigned int mRequestedIp;
};

// Response Packet
struct SASA_responsePacket{
	char mProtocolType;
	char mOpField;
	unsigned short int mServerId;
	unsigned int mChecksum;
	unsigned long int mSrcHwAddress;
	unsigned int mRequestId;
	unsigned int mAllocationValidTime;
	unsigned int mAllocatedIp;
	unsigned int mGatewayIp;
	unsigned int mSubnetMask;
	unsigned int mDnsIp;
};


#endif /* SASAPACKETS_H_ */
