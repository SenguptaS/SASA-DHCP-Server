/*
 * Settings.cpp
 *
 *  Created on: Nov 18, 2014
 *      Author: surojit
 */

#include "Settings.h"
#include <fstream>

Settings::Settings(std::string lSettingsFile) {
	this->mDBPort = 3306;
	this->mServerPort = 9191;
	mSettingsFile = lSettingsFile;
}

Settings::~Settings() {

}

void Settings::ReadSettings() {
	std::ifstream lFile;
	std::string lLine;
	lFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		lFile.open(mSettingsFile.c_str());
		std::string delim = ",";

		std::getline(lFile, lLine, ',');
		this->mDBUser = lLine;

		std::getline(lFile, lLine, ',');
		this->mDBPassword = lLine;

		std::getline(lFile, lLine, ',');
		this->mDBServer = lLine;

		std::getline(lFile, lLine, ',');
		this->mDBPort = strtoul(lLine.c_str(), NULL, 10);

		std::getline(lFile, lLine, ',');
		this->mDBSchema = lLine;

		std::getline(lFile, lLine, ',');
		this->mServerPort = strtoul(lLine.c_str(), NULL, 10);
		lFile.close();
		return;
	} catch (std::ifstream::failure &f)
	{
		throw std::logic_error(f.what());
	}
	catch (std::exception &e) {
		throw std::runtime_error(e.what());
	}
	return;
}

