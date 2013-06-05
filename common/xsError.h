/*
 * xsError.h
 *
 *  Created on: Aug 16, 2012
 *      Author: Daniel
 *      错误码文件
 */

#ifndef XSERROR_H_
#define XSERROR_H_

#include <errno.h>			// linux自带的错误码文件

/* 错误描述
 *
 */

#define XS_MSGQUEUE_FULL			1		// Message Queue full
#define	XS_LOCKWAIT_TIMEOUT			2		// Lock timeout
#define XS_MUTEXLOCK_ERROR			3		// Mutex lock request error


#endif /* XSERROR_H_ */
