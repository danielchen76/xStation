/*
 * xsDatabaseThread.h
 *
 *  Created on: Dec 20, 2012
 *      Author: Daniel
 */

#ifndef XSDATABASETHREAD_H_
#define XSDATABASETHREAD_H_

#include "../Threads/xsThread.h"

class xsDatabaseThread: public xsThread
{
public:
	xsDatabaseThread();
	virtual ~xsDatabaseThread();
};

#endif /* XSDATABASETHREAD_H_ */
