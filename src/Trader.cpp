#include "Trader.h"

Trader::Trader() : tdapi(nullptr), bIsReady(false)
{
    //loadBrokerInfo("./broker/simnow.txt", bi);
    //loadBrokerInfo("./broker/simnow_24.txt", bi);
    loadBrokerInfo("./broker/mk.txt", bi);
    
    createFolder(pTradeFlowPath);

    tdapi = CThostFtdcTraderApi::CreateFtdcTraderApi(pTradeFlowPath);
    tdapi->RegisterSpi(this);
    tdapi->RegisterFront(bi.TradeFront);
    tdapi->SubscribePublicTopic(THOST_TERT_QUICK);
    tdapi->SubscribePrivateTopic(THOST_TERT_QUICK);
    tdapi->Init();
}

Trader::~Trader() {}

///客户端认证请求
void Trader::reqAuthenticate()
{
    CThostFtdcReqAuthenticateField t{};
    strcpy(t.BrokerID, bi.BrokerID);
    strcpy(t.UserID, bi.UserID);
    strcpy(t.AuthCode, bi.AuthCode);
    strcpy(t.AppID, bi.AppID);
    
    tdapi->ReqAuthenticate(&t, ++nReqID);
}

///用户登录请求
void Trader::reqUserLogin()
{
    CThostFtdcReqUserLoginField t{};
    strcpy(t.BrokerID, bi.BrokerID);
    strcpy(t.UserID, bi.UserID);
    strcpy(t.Password, bi.Password);
    
    tdapi->ReqUserLogin(&t, ++nReqID);
}

///请求查询合约
void Trader::reqQryInstrument()
{
    CThostFtdcQryInstrumentField t{};
    
    tdapi->ReqQryInstrument(&t, ++nReqID);
}


///当客户端与交易后台建立起通信连接时(还未登录前)，该方法被调用。
void Trader::OnFrontConnected()
{
    reqAuthenticate();
}

///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
///@param nReason 错误原因
///        0x1001 网络读失败
///        0x1002 网络写失败
///        0x2001 接收心跳超时
///        0x2002 发送心跳失败
///        0x2003 收到错误报文
void Trader::OnFrontDisconnected(int nReason)
{
    printf("Trade Front Disconnected\n");
    printf("nReason = %d %s\n", nReason, nReason2str(nReason));
}

///客户端认证响应
void Trader::OnRspAuthenticate(CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo != nullptr && pRspInfo->ErrorID != 0)
    {
        printf("Failed to Authenticate\n");
        printf("ErrorID = %d ErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        return;
    }
    
    reqUserLogin();
}

///登录请求响应
void Trader::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo != nullptr && pRspInfo->ErrorID != 0)
    {
        printf("Failed to User Login\n");
        printf("ErrorID = %d, ErrorMsg : %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        return;
    }
    
    reqQryInstrument();
}

///请求查询合约响应
void Trader::OnRspQryInstrument(CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo != nullptr && pRspInfo->ErrorID != 0)
    {
        printf("Failed to Qry Instrument\n");
        printf("ErrorID = %d, ErrorMsg : %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        return;
    }
    if (pInstrument->ProductClass == THOST_FTDC_PC_Futures)
    {
        inst_vec.push_back(pInstrument->InstrumentID);
    }

    if (bIsLast)
    {
        send2WeCom("Qry Instrument Done");
        bIsReady.store(true, std::memory_order_release);
    }
}
