/*
 * Utility.cpp
 *
 *  Created on: Nov 26, 2014
 *      Author: root
 */

#include "Utility.h"

char Utility::macBuffer[128];

Utility::Utility() {
	// TODO Auto-generated constructor stub

}

Utility::~Utility() {
	// TODO Auto-generated destructor stub
}

unsigned int Utility::GetChecksum(char* pBuffer,unsigned int size)
{
	unsigned int lChecksum =0;

	for(unsigned int x=0;x<size;x++)
	{
		lChecksum += pBuffer[x];
		lChecksum = lChecksum % 0xFFFFFFFF;
	}
	return lChecksum;
}

bool Utility::VerifyChecksum(char* pBuffer,unsigned int size,unsigned int checksum)
{
	unsigned int lCaclulatedChecksum = Utility::GetChecksum(pBuffer,size);
	if(lCaclulatedChecksum == checksum)
	{
		return true;
	}
	return false;
}

char* Utility::GetPrintableMac(char* pMacIn)
{
		sprintf(Utility::macBuffer,"%02X:%02X:%02X:%02X:%02X:%02X",
				(unsigned char)pMacIn[0],(unsigned char)pMacIn[1],(unsigned char)pMacIn[2],
				(unsigned char)pMacIn[3],(unsigned char)pMacIn[4],(unsigned char)pMacIn[5],(unsigned char)pMacIn[6]);

		return Utility::macBuffer;
}


