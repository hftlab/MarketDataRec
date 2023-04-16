#ifndef TRADER_H
#define TRADER_H

#include "ThostFtdcTraderApi.h"
#include "common.h"

#include <atomic>

class Trader : public CThostFtdcTraderSpi
{
public:
	Trader();
	~Trader();

public:
	void reqAuthenticate();//客户端认证请求
	void reqUserLogin();//用户登录请求
	void reqQryInstrument();//请求查询合约

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

	///客户端认证响应
	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///请求查询合约响应
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

public:
	std::vector<std::string> inst_vec;
	std::atomic<bool> bIsReady{ false };

public:
	BrokerInfo bi;
	CThostFtdcTraderApi* tdapi;
	int nReqID = 0;
};

#endif
