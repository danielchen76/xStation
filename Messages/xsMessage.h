/*
 * xsMessage.h
 *
 *  Created on: Aug 13, 2012
 *      Author: Daniel
 *      所有处理消息（请求、回应）的基类。
 */

#ifndef XSMESSAGE_H_
#define XSMESSAGE_H_

#include <time.h>

class xsThread;

class xsMessage
{
public:
	xsMessage(xsThread* source);
	virtual ~xsMessage();

	unsigned int		m_unSerial;			// 序号(per thread)
	time_t				m_timestamp;		// 消息准备放入队列的时间（不是生成的时间，但基本上接近）
	xsThread*			m_MsgSource;		// 消息来源线程，用于响应消息回复时使用（如果有响应消息）
	
	xsThread*			m_MsgDest;			// 
	int					m_nMsgID;
};

#endif /* XSMESSAGE_H_ */
