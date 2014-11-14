
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

//Discover Packet
struct DiscoverPacket{
	char mOpField;	//1 byte
	char mHeaderType; // 1 byte
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

