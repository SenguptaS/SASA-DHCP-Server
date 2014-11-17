/*
 * databaseConnection.cpp
 *
 *  Created on: Nov 16, 2014
 *      Author: surojit
 */

#include "databaseConnection.h"

databaseConnection::databaseConnection(){
	mPLogger = Logger::getLogger(ROOT_LOGGER);
	mDatabase = "sasaDb";
	mServer = "127.0.0.1";
	mUsrername = "root";
	mPassword = "root123";
	port = 0;
}

int databaseConnection::createConnection(){

	if(mConn.connect(mDatabase.c_str(), mServer.c_str(), mUsrername.c_str(), mPassword.c_str(), port)){
		LOG4CXX_INFO(mPLogger,"Connection with IP Pool database successfully established..");
		return 0;
	}
	else{
		LOG4CXX_ERROR(mPLogger, "Connection with IP Pool could not be established at..");
		return -1;
	}

}

int databaseConnection::fireQuery(){

	if(!createConnection()){
		if((mQuery.is_null()) || (mQuery.size()<1)){
			LOG4CXX_ERROR(mPLogger, "Query to the database is null..could not retrieve results for null query");
			return -1;
		}
		else{
			Query lQuery = mConn.query(mQuery.c_str());
			LOG4CXX_INFO(mPLogger,"Query to the database set as "<< mQuery);
			if(this->mResult == lQuery.store()){
				LOG4CXX_INFO(mPLogger, "Data successfully retrieved from the database..");
				return 0;
			}
			else{
				LOG4CXX_ERROR(mPLogger, "Something seems wrong with the query or database...unsuccessful query execution..");
				return -1;
			}

		}
	}
	else{
		LOG4CXX_ERROR(mPLogger,"Connection to the database is unestablished..");
		return -1;
	}
}

void databaseConnection::setMQuery(const std::string lQuery){
	this->mQuery = lQuery;
}

StoreQueryResult databaseConnection::getMResult(){
	return mResult;
}
