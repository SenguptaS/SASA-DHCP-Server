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
		printf(Utility::macBuffer,"%02X:%02X:%02X:%02X:%02X:%02X",
				pMacIn[0],pMacIn[1],pMacIn[2],pMacIn[3],pMacIn[4],pMacIn[5],pMacIn[6]);

		return Utility::macBuffer;
}


