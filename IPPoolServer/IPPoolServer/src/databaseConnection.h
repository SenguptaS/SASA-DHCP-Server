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
#include <mysql++/query.h>
#include <mysql++/result.h>
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
	int fireQuery();
	void setMQuery(std::string lQuery);
	StoreQueryResult getMResult();

private:
	LoggerPtr mPLogger;
	Connection mConn;
	unsigned int port;
	string mUsrername;
	string mDatabase;
	string mPassword;
	string mServer;
	String mQuery;
	StoreQueryResult mResult;

	int createConnection();

};

#endif /* DATABASECONNECTION_H_ */
