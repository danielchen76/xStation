/*
 * testSerial1.c
 *
 *  Created on: Jan 7, 2013
 *      Author: Daniel
 */

#include "testSerial1.h"
#include "../common/xsGlobal.h"

void TestSerial1()
{
	void*			status;

	g_pSerialThread->Start();

	pthread_join(g_pSerialThread->GetThread(), &status);
}
