/*
 * PoolResponse.cpp
 *
 *  Created on: Nov 20, 2014
 *      Author: ateiv
 */

#include "PoolResponse.h"
#include "sasaPackets.h"
#include<string.h>
#include "DHCPPackets.h"
#include<sys/types.h>
#include<ifaddrs.h>
#include <log4cxx/logger.h>
#include<sys/socket.h>
#include<netdb.h>

PoolResponse::PoolResponse() {
	pLogger = log4cxx::Logger::getLogger("PoolResponse");
}

int PoolResponse::ProcessIPOffer() //< SASA POOL PACKET >)
{
	ResponsePacket p;

	memset(&p,0,sizeof(ResponsePacket));

	struct SASA_responsePacket q;

	struct ifaddrs *pIFList,*pFa;
	int lNoIPSReturned = getifaddrs(&pIFList);
	int n,family,a;

	if (getifaddrs(&pIFList) == -1)
	{
		LOG4CXX_ERROR(this->pLogger," Error - Server Address can't be find ");
	}
	// G through the linked list

	for (pFa = pIFList,n=0;pFa!= NULL;pFa->ifa_next,n++)
	{
		if(pFa->ifa_addr == NULL)
			continue;
		family = pFa->ifa_addr->sa_family;

		// display the address

		if (family == AF_INET){
			sockaddr_in* pSockAddrInAddr = (sockaddr_in*) pFa->ifa_addr;

//			pSockAddrInAddr->sin_addr.s_addr; //This is your ip
			break;
		}

	}

	p.mOpField = q.mOpField;
	p.mClientHardwareAddress = q.mSrcHwAddress;
	p.mTransactionId = q.mRequestId;
	p.mClientHardwareAddress = q.mSrcHwAddress;
	p.mGatewayAddress = q.mGatewayIp;
	p.mYourAddress = q.mAllocatedIp;
	p.mHeaderLength = 0x06;
	p.mHeaderType = 0x01;
	p.mServerAddress = 0;
	unsigned int lTotalBytes =0;

	char buffer[1024];
	memcpy( (void*) buffer, (const void*) &p , sizeof(ResponsePacket));

	char* pOptionsPtr = &buffer[sizeof(ResponsePacket)];

	OPHeader *pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 51;
	pOPHeader->OPLength = 4;
	memcpy((&pOPHeader+2), (const char*) &q.mAllocationValidTime, 4 );
	pOptionsPtr += 4+1;

	pOPHeader = (OPHeader*) pOptionsPtr;
	pOPHeader->OPCode = 1;
	pOPHeader->OPLength = 4;
	memcpy((&pOPHeader+2), (const char*) &q.mSubnetMask, 4 );



}

PoolResponse::~PoolResponse() {
	// TODO Auto-generated destructor stub
}

