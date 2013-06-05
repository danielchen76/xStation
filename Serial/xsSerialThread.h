/*
 * xsSerialThread.h
 *
 *  Created on: Dec 20, 2012
 *      Author: Daniel
 */

#ifndef XSSERIALTHREAD_H_
#define XSSERIALTHREAD_H_

#include "../Threads/xsThread.h"
#include "../common/xsConst.h"
#include <list>

class xsSerialThread: public xsThread
{
public:
	xsSerialThread();
	virtual ~xsSerialThread();

	// 消息处理函数
	int TestMsg(const xsMessage* pMessage);

protected:
	virtual void ProcessMessage(xsMessage* pMessage);
	virtual bool Init();
	virtual void Destroy();
	virtual void QueueEmpty();

private:
	int				m_nSerial;				// 串口文件句柄

	unsigned char	m_ucReadBuffer[SERIAL_READ_BUFFER_LENGTH];		// 读取缓冲区
	unsigned char	m_ucReadDataLength;								// 当前缓冲区内的数据字节数


	void DecodeMessage();
	bool CheckMessage();
	void MoveData(unsigned char Pos);
};

#endif /* XSSERIALTHREAD_H_ */
