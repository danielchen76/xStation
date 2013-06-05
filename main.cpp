/*
 * main.cpp
 *
 *  Created on: Aug 12, 2012
 *      Author: Daniel
 */

#include "./Serial/xsSerialThread.h"
#include <glog/logging.h>

#include "./Test/testSerial1.h"

// 全局线程变量（动态创建对象）

// 串口数据收发线程
xsSerialThread*			g_pSerialThread = NULL;

int main()
{
	// 初始化日志模块
	google::InitGoogleLogging("");
	LOG(INFO) << "System start.";

	// 初始化显示模块

	// 初始化数据库模块
	g_pSerialThread = new xsSerialThread();

	// 初始化串口通信模块


	// 测试程序
	TestSerial1();

	return 0;
}
