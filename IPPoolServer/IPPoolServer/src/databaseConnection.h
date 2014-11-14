/*
 * databaseConnection.h
 *
 *  Created on: Nov 14, 2014
 *      Author: aditi
 */

#ifndef DATABASECONNECTION_H_
#define DATABASECONNECTION_H_

#include "IPPoolServerConstants.h"
#include <mysql++/connection.h>
#include <log4cxx/logger.h>
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;
using namespace mysqlpp;
using namespace log4cxx;


class databaseConnection{

public:

	databaseConnection();
	void createConnection();

private:
	LoggerPtr mPLogger;
	Connection mConn;
	unsigned int port;
	string mUsrername;
	string mDatabase;
	string mPassword;
	string mServer;

};

databaseConnection::databaseConnection(){
	mPLogger = Logger::getLogger(ROOT_LOGGER);
	mDatabase = "sasaDb";
	mServer = "127.0.0.1";
	mUsrername = "root";
	mPassword = "root123";
	port = 0;
}

void databaseConnection::createConnection(){

	if(mConn.connect(mDatabase.c_str(), mServer.c_str(), mUsrername.c_str(), mPassword.c_str(), port)){
		LOG4CXX_INFO(mPLogger,"Connection with IP Pool database successfully established..");
	}
	else{
		LOG4CXX_ERROR(mPLogger, "Connection with IP Pool could not be established at..");
	}

}






#endif /* DATABASECONNECTION_H_ */
