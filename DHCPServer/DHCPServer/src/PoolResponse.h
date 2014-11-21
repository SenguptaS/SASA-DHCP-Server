/*
 * PoolResponse.h
 *
 *  Created on: Nov 20, 2014
 *      Author: ateiv
 */

#ifndef POOLRESPONSE_H_
#define POOLRESPONSE_H_

class PoolResponse {
public:
	PoolResponse();
	virtual ~PoolResponse();
	int ProcessIPOffer();


private:
#pragma pack(1)
	struct OPHeader
	{
		char OPCode;
		char OPLength;
	};
#pragma pack(0)


};

#endif /* POOLRESPONSE_H_ */
