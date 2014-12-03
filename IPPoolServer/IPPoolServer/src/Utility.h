/*
 * Utility.h
 *
 *  Created on: Nov 26, 2014
 *      Author: root
 */

#ifndef UTILITY_H_
#define UTILITY_H_
#include <stdio.h>

class Utility {
public:
	static char macBuffer[128];
	static char* GetPrintableMac(char* pMacIn);
	static unsigned int GetChecksum(char* pBuffer, unsigned int size);
	static bool VerifyChecksum(char* pBuffer, unsigned int size,
			unsigned int checksum);
	Utility();
	virtual ~Utility();
};

#endif /* UTILITY_H_ */
