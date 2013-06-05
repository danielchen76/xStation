/*
 * xsStruct.h
 *
 *  Created on: Dec 21, 2012
 *      Author: Daniel
 *
 *      公共结构
 */

#ifndef XSSTRUCT_H_
#define XSSTRUCT_H_

// 分机（探头）接收到的裸数据（还需要在Database模块中进行转换，例如：温度湿度计算、AD转换器数值换算）
struct xsExtRawData
{
	unsigned short		usExtID;		// 分机编号

	unsigned short		usSerial;
	short				sSolar;			// 太阳能电池电压
	short				sBattery;		// 电池电压
	short				sCharge;		// 充电电流（在1Ω电阻上的电压）
	short				sTemperature;	// 温度
	short				sHumidity;		// 湿度
	unsigned short		usState;		// 其他状态（是否充电、LED是否开启、电池低压指示）
};

// 分机数据（已经转换过的）
struct xsExtData
{
	time_t				timeStamp;		// 时间（UTC）

	unsigned short		usExtID;		// 分机编号

	unsigned short		usSerial;
	short				sSolar;			// 太阳能电池电压
	short				sBattery;		// 电池电压
	short				sCharge;		// 充电电流（在1Ω电阻上的电压）
	short				sTemperature;	// 温度
	short				sHumidity;		// 湿度
	union uState
	{
		unsigned short		usState;		// 其他状态（是否充电、LED是否开启、电池低压指示）

		struct
		{
			unsigned short		bLEDOn:1;
			unsigned short		bCharge:1;
		}
	};
};

#endif /* XSSTRUCT_H_ */
