/*
 * ThreadPassable.h
 *
 *  Created on: Nov 21, 2014
 *      Author: surojit
 */

#ifndef THREADPASSABLE_H_
#define THREADPASSABLE_H_

#include <netinet/in.h>
#include "Settings.h"

class ThreadPassable {
public:
	ThreadPassable(const Settings &lSettings);
	virtual ~ThreadPassable();

	int mClientSocket;
	sockaddr_in mClientIP;
	const Settings& mSettings;

};

#endif /* THREADPASSABLE_H_ */
