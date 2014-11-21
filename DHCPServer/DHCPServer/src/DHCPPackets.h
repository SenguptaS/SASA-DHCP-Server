//The DHCP packet is as follows
/*
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |     op (1)    |   htype (1)   |   hlen (1)    |   hops (1)    |
 +---------------+---------------+---------------+---------------+
 |                            xid (4)                            |
 +-------------------------------+-------------------------------+
 |           secs (2)            |           flags (2)           |
 +-------------------------------+-------------------------------+
 |                          ciaddr  (4)                          |
 +---------------------------------------------------------------+
 |                          yiaddr  (4)                          |
 +---------------------------------------------------------------+
 |                          siaddr  (4)                          |
 +---------------------------------------------------------------+
 |                          giaddr  (4)                          |
 +---------------------------------------------------------------+
 |                                                               |
 |                          chaddr  (16)                         |
 |                                                               |
 |                                                               |
 +---------------------------------------------------------------+
 |                                                               |
 |                          sname   (64)                         |
 +---------------------------------------------------------------+
 |                                                               |
 |                          file    (128)                        |
 +---------------------------------------------------------------+
 |                                                               |
 |                          options (variable)                   |
 +---------------------------------------------------------------+


 */
#pragma pack(push,1)
//Discover Packet
struct DiscoverPacket {
	char mOpField;	//1 byte
	char mHeaderType; // 1 byte
	char mHeaderLength; // 1 byte
	char mHops; // 1 byte
	unsigned int mTransactionId; // 4 byte
	unsigned short mSeconds; // 2 byte
	unsigned short mFlags; // 2 byte
	unsigned int mClientAddress; // 4 byte
	unsigned int mYourAddress; // 4 byte
	unsigned int mServerAddress; // 4 byte
	unsigned int mGatewayAddress; // 4 byte
	char mClientHardwareAddress[16]; // 16 byte
	char mServerName[64]; // 64 byte
	char mBootFileName[128]; // 128 byte
	char mMagicCookie[4]; // 4 Bytes
};

//Request Packet
struct RequestPacket {
	char mOpField;	//1 byte
	char mHeaderType; // 1 byte
	char mHeaderLength; // 1 byte
	char mHops; // 1 byte
	unsigned int mTransactionId; //4 byte
	unsigned short mSeconds; // 2 byte
	unsigned short mFlags; // 2 byte
	unsigned int mClientAddress; // 4 byte
	unsigned int mYourAddress; // 4 byte
	unsigned int mServerAddress; // 4 byte
	unsigned int mGatewayAddress; // 4 byte
	unsigned long int mClientHardwareAddres; // 16 byte
	char mServerName[64]; // 64 byte
	char mBootFileName[128]; // 128 byte

};
//Response Packet
struct ResponsePacket {
	char mOpField;	//1 byte
	char mHeaderType; // 1 byte
	char mHeaderLength; // 1 byte
	char mHops; //1 byte
	unsigned int mTransactionId; // 4 byte
	unsigned short mSeconds; // 2 byte
	unsigned short mFlags; // 2 byte
	unsigned int mClientAddress; // 4 byte
	unsigned int mYourAddress; // 4 byte
	unsigned int mServerAddress; // 4 byte
	unsigned int mGatewayAddress; // 4 byte
	unsigned long int mClientHardwareAddress; // 16 byte
	char mServerName[64]; // 64 byte
	char mBootFileName[128]; // 128 byte

};

//Acknowledgment Packet
struct AcknowledgmentPacket {
	char mOpField;	//1 byte
	char mHeaderType; // 1 byte
	char mHeaderLength; // 1 byte
	char mHops; // 1 byte
	unsigned int mTransactionId; // 4 byte
	unsigned short mSeconds; // 2 byte
	unsigned short mFlags; // 2 byte
	unsigned int mClientAddress; // 4 byte
	unsigned int mYourAddress; // 4 byte
	unsigned int mServerAddress; // 4 byte
	unsigned int mGatewayAddress; // 4 byte
	unsigned long int mClientHardwareAddress; // 16 byte
	char mServerName[64]; // 64 byte
	char mBootFileName[128]; // 128 byte
};
//Request Packet Pool Side
struct RequestPacketPS {

	char mProtocolType; //1 byte
	unsigned short mOpField; //1 byte
	unsigned short mServerId; //2 byte
	unsigned int mChecksum; //4 byte
	char mSourceHardwareAddress[6]; //6 byte
	unsigned int mRequestId; //4 byte
	unsigned int mPreviousIP; //4 byte

};

struct ResponsePacketPS {
	char mProtocolType; //1 byte
	unsigned short mOpField;//1 byte
	unsigned short mServerId;//2 byte
	unsigned int mChecksum;//4 byte
	char mSourceHardwareAddress[6];//6 byte
	unsigned int mRequestId;//4 byte
	unsigned int mAllocationValidTill;////4 byte
	unsigned int mAllocatedIP;//4 byte
	unsigned int mGatewayAddress;//4 byte
	unsigned int mSubnetMask;//4 byte
	unsigned int mDns;//4 byte
};

#pragma pack(pop)
