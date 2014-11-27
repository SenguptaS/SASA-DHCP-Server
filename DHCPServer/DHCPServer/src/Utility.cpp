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


char* Utility::GetPrintableMac(char* pMacIn)
{

		sprintf(Utility::macBuffer,"%02X:%02X:%02X:%02X:%02X:%02X",
				(unsigned char) pMacIn[0],(unsigned char) pMacIn[1], (unsigned char)pMacIn[2],(unsigned char)pMacIn[3],
				(unsigned char)pMacIn[4],(unsigned char)pMacIn[5],(unsigned char)pMacIn[6]);
		return Utility::macBuffer;
}


