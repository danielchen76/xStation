/*
 * xsThread.h
 *
 *  Created on: Aug 14, 2012
 *      Author: Daniel
 *
 *      所有处理线程的基类。主要处理消息的接收和处理的标准入口。
 */

#ifndef XSTHREAD_H_
#define XSTHREAD_H_

#include <queue>
#include <pthread.h>
#include <limits.h>

#include "../common/xsError.h"

#include "../Messages/xsMessage.h"

#define MAX_THREAD_NAME_LENGTH		10

class xsThread
{
public:
	xsThread(const char* szName);
	virtual ~xsThread();

	bool Start();												// 启动线程
	bool Stop();												// 停止线程
	bool m_bStop;

	int AddMessage(xsMessage* pMessage);						// 向消息队列放入消息
	int GetMessage(xsMessage*& pMessage, bool& bQueueEmpty);	// 从消息队列读取消息

	int SetMaxMsgs(int number);									// 设置消息队列最大容量

	unsigned int GetNewSerial();

	pthread_t GetThread() {return m_thread_t;}

protected:
	std::queue<xsMessage*>				m_MessageQueue;			// 消息队列
	pthread_mutex_t						m_MQMutex;				// 消息队列的锁
	bool								m_bHaveMsg;				// 队列是否有消息条件
	pthread_cond_t						m_HaveMsgEvent;			// 消息队列有消息事件

	int									m_nMaxMsgs;				// 消息队列最大上限

	pthread_t							m_thread_t;				// 线程句柄

	virtual bool Init() = 0;									// 初始化对象，同时初始化内部线程
	virtual void Destroy() = 0;									// 停止内部线程
	virtual void QueueEmpty() {}								// 消息队列为空的虚函数，可以做一些业务相关的定时事务
	virtual void ProcessMessage(xsMessage* pMessage) = 0;
	static void* ThreadProc(void* ptr);

private:
	unsigned int						m_unCurrentSerial;		// 当前序列号
	char								m_szThreadName[MAX_THREAD_NAME_LENGTH];		// 当前线程名称

};

template <class T>
class MessageProcessFunction
{
	typedef int (T::*pfMessageProc)(const xsMessage* pMessage);
public:
	MessageProcessFunction(int msgID, pfMessageProc pfunction)
	{
		nMsgID		= msgID;
		pFunction	= pfunction;
	}

//	MessageProcessFunction(const MessageProcessFunction<T>& x)
//	{
//		*this = x;
//	}

	MessageProcessFunction<T>& operator=(const MessageProcessFunction<T>& x)
	{
		nMsgID		= x.nMsgID;
		pFunction	= x.pFunction;
	}

	int				nMsgID;
	pfMessageProc	pFunction;

};

#endif /* XSTHREAD_H_ */
