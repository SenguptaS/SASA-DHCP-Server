//============================================================================
// Name        : IPPoolServer.cpp
// Author      : Surojit Sengupta
// Version     :
// Copyright   : All Rights Reserved 2014
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include "databaseConnection.h"
#include "IPPoolServerConstants.h"
#include <unistd.h>
#include <cerrno>
#include <errno.h>
#include "Settings.h"

using namespace std;

int main(int argc, char *argv[]) {

	int lSocketFd, lNewSocketFd, lPortNo, lNoOfCharRead;
	char lbuffer[BUFFER_SIZE];
	struct sockaddr_in lServerAddr;
	struct sockaddr_in lClientAddr;
	socklen_t lClientAddressSize = sizeof(sockaddr_in);

	log4cxx::PropertyConfigurator::configure("log.cfg");
	log4cxx::LoggerPtr pLogger = log4cxx::Logger::getLogger(ROOT_LOGGER);
	time_t mTimeNow = time(NULL);

	if (argc < 3) {
		LOG4CXX_FATAL(pLogger,
				"No configuration file provided. Usage: IPPoolServer -c <config_file>");
		return EXIT_FAILURE;
	}

	if (strcmp(argv[1], "-c") != 0) {
		LOG4CXX_FATAL(pLogger, "Invalid option " << argv[1]);
		return EXIT_FAILURE;
	}

	if (strlen(argv[2]) <= 0) {
		LOG4CXX_FATAL(pLogger,"Invalid or No configuration file provided");
		return EXIT_FAILURE;
	}

	Settings lSettings(argv[2]);
	try
	{
		lSettings.ReadSettings();
	}
	catch(std::exception &ex)
	{
		LOG4CXX_FATAL(pLogger,"Failed to read settings file - " << ex.what());
		return EXIT_FAILURE;
	}

	LOG4CXX_INFO(pLogger, "Read settings file - " << argv[2]);

	LOG4CXX_INFO(pLogger, "IPPool Starting up - " << ctime(&mTimeNow));

	lSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (lSocketFd < 0) {
		LOG4CXX_ERROR(pLogger, "Error opening the socket");
	}

	bzero((char*) &lServerAddr, sizeof(lServerAddr));

	lServerAddr.sin_family = AF_INET;
	lServerAddr.sin_port = htons(65500);
	lServerAddr.sin_addr.s_addr = 0;

	if (bind(lSocketFd, (struct sockaddr*) &lServerAddr, sizeof(lServerAddr))) {
		LOG4CXX_ERROR(pLogger,
				"Socket binding failed...try some other port no.");
	} else {
		listen(lSocketFd, 5);
		lNewSocketFd = accept(lSocketFd, (struct sockaddr*) &lClientAddr,
				&lClientAddressSize);
		if (lNewSocketFd < 0) {
			LOG4CXX_ERROR(pLogger,
					"Failed to establish a connection with the client - " << strerror(errno));
		} else {
			LOG4CXX_INFO(pLogger,
					"Successfully established a connection with the client..");
			bzero(lbuffer, BUFFER_SIZE);
			lNoOfCharRead = read(lNewSocketFd, lbuffer, BUFFER_SIZE);
			if (lNoOfCharRead < 0) {
				LOG4CXX_ERROR(pLogger, "Error reading from the socket");
			} else {
				LOG4CXX_INFO(pLogger,
						"Message read from the socket -->" << lbuffer);
			}
		}
		close(lSocketFd);
	}

	return 0;
}
