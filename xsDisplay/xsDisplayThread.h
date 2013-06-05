/*
 * xsDisplayThread.h
 *
 *  Created on: Aug 14, 2012
 *      Author: Daniel
 */

#ifndef XSDISPLAYTHREAD_H_
#define XSDISPLAYTHREAD_H_

#include "../Threads/xsThread.h"
#include <list>

class xsDisplayThread: public xsThread
{
public:
	xsDisplayThread();
	virtual ~xsDisplayThread();

	// 消息处理函数
	int TestMsg(const xsMessage* pMessage);			// 测试消息

protected:
	virtual void ProcessMessage(xsMessage* pMessage);
	virtual bool Init();
	virtual void Destroy();

private:

};

#endif /* XSDISPLAYTHREAD_H_ */
