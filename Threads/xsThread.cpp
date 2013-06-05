/*
 * xsThread.cpp
 *
 *  Created on: Aug 14, 2012
 *      Author: Daniel
 */

#include "xsThread.h"
#include <string.h>
#include <time.h>
#include <glog/logging.h>

// 下面代码用于

xsThread::xsThread(const char* szName)
{
	// 初始化消息队列锁
	pthread_mutex_init(&m_MQMutex, NULL);

	m_bHaveMsg = false;
	pthread_cond_init(&m_HaveMsgEvent, NULL);

	// 消息队列最大容量（默认为200个）
	m_nMaxMsgs = 200;

	m_unCurrentSerial = 0;

	memset(m_szThreadName, 0, sizeof(m_szThreadName));
	strncpy(m_szThreadName, szName, MAX_THREAD_NAME_LENGTH - 1);
}

xsThread::~xsThread()
{
	pthread_mutex_destroy(&m_MQMutex);
	pthread_cond_destroy(&m_HaveMsgEvent);
}

bool xsThread::Start()
{
	int		ret = 0;
	
	m_bStop = false;

	// 创建线程
	ret = pthread_create(&m_thread_t, NULL, &ThreadProc, this);

	if (ret != 0)
	{
		LOG(ERROR) << "Start thread(" << m_szThreadName << ") failed.";
		return false;
	}

	return true;;
}

bool xsThread::Stop()
{
	m_bStop = true;
	return true;
}

void* xsThread::ThreadProc(void* ptr)
{
	if (ptr == NULL)
	{
		return 0;
	}

	xsThread*	pThread = (xsThread*)ptr;

	// 运行线程初始化函数
	if (!pThread->Init())
	{
		// 派生类初始化过程失败，退出线程
		LOG(ERROR) << "Init thread(" << pThread->m_szThreadName << ") failed.";
		return 0;
	}

	xsMessage*		pMessage = NULL;
	int				ret;
	bool			bQueueEmpty = false;

	// 循环处理消息
	while (!pThread->m_bStop)	// 检查是否退出
	{
		ret = pThread->GetMessage(pMessage, bQueueEmpty);
		if ((ret == 0) && (pMessage))
		{
			// 有消息，处理消息
			pThread->ProcessMessage(pMessage);
		}

		if (bQueueEmpty)
		{
			pThread->QueueEmpty();
		}
	}

	// 退出前调用虚拟函数
	pThread->Destroy();

	return (void*)0;
}

int xsThread::AddMessage(xsMessage *pMessage)
{
	int		ret		= 0;
	int		mqSize	= 0;

	if (!pMessage)
	{
		return -1;
	}

	// Lock
	pthread_mutex_lock(&m_MQMutex);

	mqSize = m_MessageQueue.size();

	if (mqSize >= m_nMaxMsgs)
	{
		ret = XS_MSGQUEUE_FULL;
	}
	else
	{
		m_MessageQueue.push(pMessage);
		mqSize++;
		m_bHaveMsg = true;
	}

	pthread_mutex_unlock(&m_MQMutex);

	// 判断档mqSize＝一个门限时，记录日志，提示消息队列接近容限。
	LOG_IF(WARNING, mqSize == (m_nMaxMsgs - 10)) << "Message Queue maybe full (" << m_szThreadName << ")";

	if (ret != 0)
	{
		// 记录日志
		LOG(ERROR) << "Add message failed(" << ret << ").";
	}
	else
	{
		// 通知读取函数有消息在队列中
		pthread_cond_signal(&m_HaveMsgEvent);
	}

	return ret;
}

int xsThread::GetMessage(xsMessage *& pMessage, bool& bQueueEmpty)
{
	int				ret;
	int				mqSize;
	timespec		ts;

	clock_gettime(CLOCK_REALTIME, &ts);
	//ts.tv_sec 	= 0;
	ts.tv_nsec	+= 100 * 1000000;			// 100ms

	pMessage = NULL;
	bQueueEmpty = true;

	// Lock
	pthread_mutex_lock(&m_MQMutex);

	// 判断是否有消息
	if (!m_bHaveMsg)
	{
		// 没有消息，等待信号量（有消息），同时请求队列锁
		ret = pthread_cond_timedwait(&m_HaveMsgEvent, &m_MQMutex, &ts);

		if (ret == ETIMEDOUT)
		{
			pthread_mutex_unlock(&m_MQMutex);
			return XS_LOCKWAIT_TIMEOUT;
		}
		else if (ret != 0)
		{
			pthread_mutex_unlock(&m_MQMutex);
			return XS_MUTEXLOCK_ERROR;
		}
	}

	// 有消息
	mqSize = m_MessageQueue.size();
	if (mqSize > 0)
	{
		pMessage = m_MessageQueue.front();
		m_MessageQueue.pop();
	}

	if (mqSize == 1)		// 最后一条消息，清除条件
	{
		m_bHaveMsg 	= false;
		bQueueEmpty = true;		// 消息队列为空
	}
	else
	{
		bQueueEmpty = false;
	}

	pthread_mutex_unlock(&m_MQMutex);

	return ret;
}

int xsThread::SetMaxMsgs(int number)
{
	int			nOld = m_nMaxMsgs;

	m_nMaxMsgs = number;

	return nOld;
}

// 获取新的消息序号，当前线程唯一
unsigned int xsThread::GetNewSerial()
{
	return m_unCurrentSerial++;
}





