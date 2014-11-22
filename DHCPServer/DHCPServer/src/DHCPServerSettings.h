/*
 * DHCPServerSettings.h
 *
 *  Created on: Nov 21, 2014
 *      Author: ateiv
 */

#ifndef DHCPSERVERSETTINGS_H_
#define DHCPSERVERSETTINGS_H_

class DHCPServerSettings {
public:
	static std::string mServerName;
	DHCPServerSettings();
	virtual ~DHCPServerSettings();
};

#endif /* DHCPSERVERSETTINGS_H_ */
