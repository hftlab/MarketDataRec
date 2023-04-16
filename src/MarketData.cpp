#include "MarketData.h"


MarketData::MarketData(std::vector<std::string>& inst) : mdapi(nullptr), inst_vec(inst)
{
    createFolder(pMdFlowPath);
    //loadBrokerInfo("./broker/simnow.txt", bi);
    //loadBrokerInfo("./broker/simnow_24.txt", bi);
    //loadBrokerInfo("./broker/mk.txt", bi);
    loadBrokerInfo("./broker/mk_5.txt", bi);
    //loadBrokerInfo("./broker/5.txt", bi);

    mdapi = CThostFtdcMdApi::CreateFtdcMdApi(pMdFlowPath, true, true);
    mdapi->RegisterSpi(this);
    mdapi->RegisterFront(bi.MarketFront);
    mdapi->Init();
}

MarketData::~MarketData() {}


///当客户端与交易后台建立起通信连接时(还未登录前)，该方法被调用。
void MarketData::OnFrontConnected()
{
    printf("Market Front Connected\n");
    send2WeCom("Market Front Connected");

    CThostFtdcReqUserLoginField t{};
    strcpy(t.BrokerID, bi.BrokerID);
    strcpy(t.UserID, bi.UserID);
    strcpy(t.Password, bi.Password);

    int rtn = mdapi->ReqUserLogin(&t, ++nReqID);
}

///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
///@param nReason 错误原因
///        0x1001 网络读失败
///        0x1002 网络写失败
///        0x2001 接收心跳超时
///        0x2002 发送心跳失败
///        0x2003 收到错误报文
void MarketData::OnFrontDisconnected(int nReason)
{
    printf("Market Front Disconnected\n");
    send2WeCom("Market Front Disconnected");
    printf("nReason = %d Reason = %s\n", nReason, nReason2str(nReason));

    overnight(mdapi->GetTradingDay());
}

///登录请求响应
void MarketData::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo != nullptr && pRspInfo->ErrorID != 0)
    {
        printf("ErrorID = %d ErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        return;
    }
    
    for (auto iter : inst_vec)
    {
        char* ppInsts[] = { (char*)iter.c_str() };
        mdapi->SubscribeMarketData(ppInsts, 1);
    }
}

///登出请求响应
void MarketData::OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{}

///订阅行情应答
void MarketData::OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo != nullptr && pRspInfo->ErrorID != 0)
    {
        printf("ErrorID = %d ErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        return;
    }

    static auto day = mdapi->GetTradingDay();

    if (pSpecificInstrument->InstrumentID == nullptr) { return; }

#ifdef _WIN32
    if (!std::filesystem::exists(day))
#else
    if (!access(day, F_OK) == 0)
#endif // _WIN32
    {
        createFolder(day);
    }    
    
    char fileName[64] = { '\0' };    
    sprintf(fileName, "./%s/%s_%s.csv", day, pSpecificInstrument->InstrumentID, day);

#ifdef _WIN32
    if (std::filesystem::exists(fileName)) { return; }
#else
    if (access(fileName, F_OK) == 0) { return; }
#endif // _WIN32

    std::ofstream outFile;
    outFile.open(fileName, std::ios::out); // 新开文件
    outFile << "	InstrumentID" << ","///交易所代码
        << "ExchangeID	" << ","///合约代码
        << "TradingDay" << ","///交易日
        << "UpdateTime" << ","///最后修改时间
        << "UpdateMillisec" << ","///最后修改毫秒
        << "PreSettlementPrice" << ","///上次结算价
        << "PreClosePrice" << ","///昨收盘
        << "PreOpenInterest" << ","///昨持仓量
        << "OpenPrice" << ","///今开盘
        << "UpperLimitPrice" << ","///涨停板价
        << "LowerLimitPrice" << ","///跌停板价
        << "HighestPrice" << ","///最高价
        << "LowestPrice" << ","///最低价
        << "LastPrice" << ","///最新价
        << "Volume" << ","///数量
        << "Turnover" << ","///成交金额
        << "OpenInterest" << ","///持仓量
        << "BidPrice1" << ","///申买价一
        << "BidVolume1" << ","///申买量一
        << "AskPrice1" << ","///申卖价一
        << "AskVolume1" << ","///申卖量一
        << "BidPrice2" << ","///申买价二
        << "BidVolume2" << ","///申买量二
        << "AskPrice2" << ","///申卖价二
        << "AskVolume2" << ","///申卖量二
        << "BidPrice3" << ","///申买价三
        << "BidVolume3" << ","///申买量三
        << "AskPrice3" << ","///申卖价三
        << "AskVolume3" << ","///申卖量三
        << "BidPrice4" << ","///申买价四
        << "BidVolume4" << ","///申买量四
        << "AskPrice4" << ","///申卖价四
        << "AskVolume4" << ","///申卖量四
        << "BidPrice5" << ","///申买价五
        << "BidVolume5" << ","///申买量五
        << "AskPrice5" << ","///申卖价五
        << "AskVolume5"///申卖量五
        << std::endl;
    outFile.close();
}

///深度行情通知
void MarketData::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
{
    if (pDepthMarketData != nullptr)
    {
        md.insert(*pDepthMarketData);
    }
}
