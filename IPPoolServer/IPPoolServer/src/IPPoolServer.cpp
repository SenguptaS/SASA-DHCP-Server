//============================================================================
// Name        : IPPoolServer.cpp
// Author      : Surojit Sengupta
// Version     :
// Copyright   : All Rights Reserved 2014
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>

using namespace std;

int main() {
	log4cxx::PropertyConfigurator::configure("log.cfg");
	log4cxx::LoggerPtr pLogger = log4cxx::Logger::getRootLogger();
	LOG4CXX_FATAL(pLogger,"This is a fatal error!");

	return 0;
}
