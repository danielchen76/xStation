/*
 * xsDisplayThread.cpp
 *
 *  Created on: Aug 14, 2012
 *      Author: Daniel
 */

#include "xsDisplayThread.h"

// 定义消息处理函数的列表
static MessageProcessFunction<xsDisplayThread>	s_functions[] =
		{
				MessageProcessFunction<xsDisplayThread>(0, &xsDisplayThread::TestMsg),
		};

static int	s_nfunctions = sizeof(s_functions) / sizeof(s_functions[0]);


xsDisplayThread::xsDisplayThread()
	: xsThread("Display")
{
	//
}

xsDisplayThread::~xsDisplayThread()
{
	// TODO Auto-generated destructor stub
}

bool xsDisplayThread::Init()
{
	// 打开framebuffer，设置缓冲区

	return true;
}

void xsDisplayThread::Destroy()
{
	// 关闭framebuffer
}

void xsDisplayThread::ProcessMessage(xsMessage* pMessage)
{
	std::list< MessageProcessFunction<xsDisplayThread>* >::const_iterator	it;

	for (int i = 0; i < s_nfunctions; i++)
	{
		if (s_functions[i].nMsgID == pMessage->m_nMsgID)
		{
			(this->*(s_functions[i].pFunction))(pMessage);
			return;
		}
	}
}

int xsDisplayThread::TestMsg(const xsMessage* pMessage)
{
	return 0;
}

