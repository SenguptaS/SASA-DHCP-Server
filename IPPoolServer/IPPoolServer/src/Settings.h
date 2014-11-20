/*
 * Settings.h
 *
 *  Created on: Nov 18, 2014
 *      Author: surojit
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_
#include <string>
#include <stdexcept>
#include <cstdlib>

class Settings {
public:
	Settings(std::string lSettingsFile);
	void ReadSettings();
	virtual ~Settings();
	std::string mSettingsFile;
	std::string mDBUser;
	std::string mDBPassword;
	std::string mDBServer;
	std::string mDBSchema;
	unsigned int mDBPort;
	unsigned int mServerPort;
private:

};

#endif /* SETTINGS_H_ */
