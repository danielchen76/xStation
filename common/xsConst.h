/*
 * xsConst.h
 *
 *  Created on: Jan 6, 2013
 *      Author: Daniel
 *
 *      公共常量等
 */

#ifndef XSCONST_H_
#define XSCONST_H_

// 串口路径
#define SERIAL_PATH		"/dev/s3c2410_serial1"

// 接收解码缓冲区（50字节，每个包最长不大于50，包含包头、包尾）
#define SERIAL_READ_BUFFER_LENGTH		50

#endif /* XSCONST_H_ */
