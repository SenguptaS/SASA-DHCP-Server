/*
 * ThreadPassable.h
 *
 *  Created on: Nov 21, 2014
 *      Author: surojit
 */

#ifndef THREADPASSABLE_H_
#define THREADPASSABLE_H_

#include <netinet/in.h>

class ThreadPassable {
public:
	ThreadPassable();
	virtual ~ThreadPassable();

	int mClientSocket;
	sockaddr_in mClientIP;

};

#endif /* THREADPASSABLE_H_ */
