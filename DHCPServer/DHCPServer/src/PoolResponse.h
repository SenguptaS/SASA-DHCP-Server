/*
 * PoolResponse.h
 *
 *  Created on: Nov 20, 2014
 *      Author: ateiv
 */

#ifndef POOLRESPONSE_H_
#define POOLRESPONSE_H_

#include <log4cxx/logger.h>
#include "sasaPackets.h"

class PoolResponse {

public:
	PoolResponse(unsigned short nServerIdentifier,
			std::string nInterfaceIpAddress, int nUDPSocket);
	virtual ~PoolResponse();
	int ProcessIPOffer(SASA_responsePacket* pResponsePacket,std::string nInterfaceIpAddress, int nUDPSocket);

private:
	log4cxx::LoggerPtr pLogger;
	unsigned short lServerIdentifier;
	std::string lLocalServerIPAddress;
	int lUDPSocket;

#pragma pack(1)
	struct OPHeader {
		char OPCode;
		char OPLength;
	};
#pragma pack(0)
};

#endif /* POOLRESPONSE_H_ */
