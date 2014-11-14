//============================================================================
// Name        : DHCPServer.cpp
// Author      : Surojit Sengupta
// Version     :
// Copyright   : All Rights Reserved 2014
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

//Discover Packet
struct DiscoverPacket{
	char mOpField;
	char mHeaderType;
	unsigned int mHeaderLength;
	unsigned int mHops;
	unsigned int mTransactionId;
	unsigned int mSeconds;
	char mFlags;
	unsigned int mClientAddress;
	unsigned int mYourAddress;
	unsigned int mServerAddress;
	unsigned int mGatewayAddress;
	unsigned long int mClientHardwareAddress;
	char mServerName;
	char mBootFileName;
	char mOptions;

};

//Request Packet
struct RequestPacket{
	char mOpField;
	char mHeaderType;
	unsigned int mHeaderLength;
	unsigned int mHops;
	unsigned int mTransactionId;
	unsigned int mSeconds;
	char mFlags;
	unsigned int mClientAddress;
	unsigned int mYourAddress;
	unsigned int mServerAddress;
	unsigned int mGatewayAddress;
	unsigned long int mClientHardwareAddress;
	char mServerName;
	char mBootFileName;
	char mOptions;

};

//Response Packet
struct ResponsePacket{
	char mOpField;
	char mHeaderType;
	unsigned int mHeaderLength;
	unsigned int mHops;
	unsigned int mTransactionId;
	unsigned int mSeconds;
	char mFlags;
	unsigned int mClientAddress;
	unsigned int mYourAddress;
	unsigned int mServerAddress;
	unsigned int mGatewayAddress;
	unsigned long int mClientHardwareAddress;
	char mServerName;
	char mBootFileName;
	char mOptions;
};

//Acknowledgment Packet
struct AcknowledgmentPacket{
	char mOpField;
	char mHeaderType;
	unsigned int mHeaderLength;
	unsigned int mHops;
	unsigned int mTransactionId;
	unsigned int mSeconds;
	char mFlags;
	unsigned int mClientAddress;
	unsigned int mYourAddress;
	unsigned int mServerAddress;
	unsigned int mGatewayAddress;
	unsigned long int mClientHardwareAddress;
	char mServerName;
	char mBootFileName;
	char mOptions;
};

