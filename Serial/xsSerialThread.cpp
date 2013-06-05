/*
 * xsSerialThread.cpp
 *
 *  Created on: Dec 20, 2012
 *      Author: Daniel
 *
 *      串口收发线程，接收来至MSP430的数据，各个温度、湿度探头、太阳能、电池电压、充电电流、LED灯等数据
 */

#include "xsSerialThread.h"
#include <glog/logging.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

static MessageProcessFunction<xsSerialThread>	s_functions[] =
		{
				MessageProcessFunction<xsSerialThread>(0, &xsSerialThread::TestMsg),
		};

static int	s_nfunctions = sizeof(s_functions) / sizeof(s_functions[0]);

xsSerialThread::xsSerialThread()
	: xsThread("Serial")
{
	m_nSerial = 0;

	memset(m_ucReadBuffer, 0, sizeof(m_ucReadBuffer));
	m_ucReadDataLength = 0;
}

xsSerialThread::~xsSerialThread()
{

}

void xsSerialThread::ProcessMessage(xsMessage* pMessage)
{
	std::list< MessageProcessFunction<xsSerialThread>* >::const_iterator	it;

	for (int i = 0; i < s_nfunctions; i++)
	{
		if (s_functions[i].nMsgID == pMessage->m_nMsgID)
		{
			(this->*(s_functions[i].pFunction))(pMessage);
			return;
		}
	}
}

bool xsSerialThread::Init()
{
	// 初始化串口
	// 开启串口，读写、该串口不作为控制台、不需要关心DCD信号
	m_nSerial = open(SERIAL_PATH, O_RDWR | O_NOCTTY | O_NDELAY);

	if (m_nSerial < 0)
	{
		// 串口开启失败
		LOG(ERROR) << "Open serial(" << SERIAL_PATH << ") failed, error:" << errno;
		return false;
	}
	else
	{
		LOG(INFO) << "Open serial(" << SERIAL_PATH << ") successed.";
	}

	// 设置波特率
	termios		serialOption;

	// 获取现有的串口配置，然后修改后再设置
	tcgetattr(m_nSerial, &serialOption);

	// 9600，8，N，1
	cfsetispeed(&serialOption, B9600);
	cfsetospeed(&serialOption, B9600);
	serialOption.c_cflag |= (CLOCAL | CREAD);
	serialOption.c_cflag &= ~PARENB;
	serialOption.c_cflag &= ~CSTOPB;
	serialOption.c_cflag &= ~CSIZE;			// 清除，再设置位数
	serialOption.c_cflag |= CS8;
	serialOption.c_cflag &= ~CRTSCTS;		// 禁用硬件流控
	serialOption.c_iflag &= ~(IXON | IXOFF | IXANY); //disable flow control

	serialOption.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);		// raw模式

	tcflush(m_nSerial, TCIFLUSH);
	tcsetattr(m_nSerial, TCSANOW, &serialOption);

	// 非阻塞读写
	fcntl(m_nSerial, F_SETFL, FNDELAY);

	return true;
}

void xsSerialThread::Destroy()
{
	// 关闭串口
	if (m_nSerial > 0)
	{
		close(m_nSerial);
		LOG(INFO) << "Close serial.";
	}
}

void xsSerialThread::QueueEmpty()
{
	// 系统空闲（没有消息需要处理），接收串口数据，并试图进行解码。
	// 在没有接收到数据后，再退出。（正常是不会一直有数据的）
	int		nRead;

	while (true)
	{
		nRead = read(m_nSerial, m_ucReadBuffer + m_ucReadDataLength, sizeof(m_ucReadBuffer) - m_ucReadDataLength);

		if (nRead == 0)
		{
			// 没有数据
			return;
		}
		else if (nRead < 0)
		{
			LOG(WARNING) << "Read serial data failed, error:" << errno;
			return;
		}

		m_ucReadDataLength += nRead;	// 缓冲区增加的数据

		// 对缓冲区试图解码
		DecodeMessage();
	}
}

// 对当前缓冲区内的数据进行解码（包头标识搜索，完整性判断，分解消息）
void xsSerialThread::DecodeMessage()
{
	unsigned char			Pos;
	unsigned char			dataLength = 0;


	while (true)
	{
		// 搜索“FE AA FE 55”的包头
		// 搜索FE，如果没有，缓冲区内的数据全部丢弃
		for (Pos = 0; Pos < m_ucReadDataLength; ++Pos)
		{
			if (m_ucReadBuffer[Pos] == 0xFE)
			{
				break;
			}
		}

		if (Pos == m_ucReadDataLength)
		{
			// 没有找到FE，丢弃所有数据
			m_ucReadDataLength = 0;
			return;
		}

		// 向前移动数据
		if (Pos != 0)
		{
			MoveData(Pos);
		}

		// 开始判断头部后续的数据
		if (m_ucReadDataLength < 5)
		{
			// 至少有4个字节才进行判断（头部4字节标识+1字节长度，这样才能进行有消息判断）
			return;
		}

		if ((m_ucReadBuffer[1] == 0xAA)
			&& (m_ucReadBuffer[2] == 0xFE)
			&& (m_ucReadBuffer[3] == 0x55))
		{
			// 获取长度
			dataLength = m_ucReadBuffer[4];

			// 判断缓冲区是否有足够数据，没有足够数据就不需要进行后续解码判断
			if (dataLength > (SERIAL_READ_BUFFER_LENGTH - 8))
			{
				// 无法支持太大的数据包解码（可能是头部刚好和数据中的数据一样，但是长度又太长了）
				m_ucReadBuffer[0] = 0;		// 强制清除第一个字节，使得不会在下次循环判断到是个正确的头部
				continue;
			}

			if (m_ucReadDataLength < (dataLength + 8))
			{
				// 还没有足够的长度（8字节是4个头部+4个尾部）
				return;
			}

			// 有足够的数据，可以进行包完整性校验。如果校验失败，则清除第一个字节。如果成功，则向前移动数据，继续解码剩余的数据
			if (CheckMessage())
			{
				// 移动数据，继续解码
				MoveData(dataLength + 8);
				continue;
			}
			else
			{
				m_ucReadBuffer[0] = 0;
				continue;
			}

		}
		else
		{
			// 不是头部，那么第一个字节的FE就没有用处了，强制为00，然后循环重新找FE
			m_ucReadBuffer[0] = 0;
			continue;
		}
	}
}

bool xsSerialThread::CheckMessage()
{
	// 检查尾部
	// 获取包数据长度

	// 数据校验争取，解析后，根据消息类型，发送到对应线程处理。

	return false;
}

void xsSerialThread::MoveData(unsigned char Pos)
{
	int		i;
	int		j = 0;

	for (i = Pos; i < m_ucReadDataLength; ++i)
	{
		m_ucReadBuffer[j] = m_ucReadBuffer[i];
		j++;
	}
	m_ucReadDataLength -= Pos;		// 减去 移走的数据长度
}

int xsSerialThread::TestMsg(const xsMessage* pMessage)
{
	return 0;
}



