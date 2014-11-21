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

PoolResponse::PoolResponse() {
	// TODO Auto-generated constructor stub

}

int PoolResponse::ProcessIPOffer() //< SASA POOL PACKET >)
{
	ResponsePacket p;
	struct SASA_responsePacket q;
	p.mOpField = q.mOpField;
	p.mClientHardwareAddress = q.mSrcHwAddress;
	p.mTransactionId = q.mRequestId;
	p.mClientHardwareAddress = q.mSrcHwAddress;
	p.mGatewayAddress = q.mGatewayIp;
	p.mYourAddress = q.mAllocatedIp;

	char buffer[1024];
	memcpy( (void*) buffer, (const void*) &q , 48);

	char* pOptionsPtr = &buffer[sizeof(SASA_responsePacket)];

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

