/*
 * xsMessage.cpp
 *
 *  Created on: Aug 13, 2012
 *      Author: Daniel
 */

#include <assert.h>
#include "xsMessage.h"
#include "../Threads/xsThread.h"

xsMessage::xsMessage(xsThread* source)
{
	assert(source);
	// m_timestamp = time(null);
	m_MsgSource = source;

	if (source)
	{
		m_unSerial = source->GetNewSerial();
	}
}

xsMessage::~xsMessage()
{
	// TODO Auto-generated destructor stub
}
