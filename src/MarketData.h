#ifndef MARKET_DATA_H
#define MARKET_DATA_H

#include "ThostFtdcMdApi.h"
#include "common.h"
#include "ringbuffer.h"


class MarketData : public CThostFtdcMdSpi
{
public:
	MarketData(std::vector<std::string>& inst);
	~MarketData();

public:
	///当客户端与交易后台建立起通信连接时(还未登录前)，该方法被调用。
	virtual void OnFrontConnected();

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason);

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData);

public:
	ringbuffer<CThostFtdcDepthMarketDataField, 1024> md;

public:
	CThostFtdcMdApi* mdapi;
	BrokerInfo bi;
	int nReqID = 0;
	std::vector<std::string> inst_vec;
};

#endif
