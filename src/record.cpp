#include "record.h"

record::record() : td_ptr(nullptr), md_ptr(nullptr)
{
    td_ptr = new Trader();
    while (!td_ptr->bIsReady.load(std::memory_order_acquire)) {}

    md_ptr = new MarketData(td_ptr->inst_vec);

    td_ptr->tdapi->Release(); delete td_ptr;

	start();
}

record::~record()
{
    delete md_ptr;
}

void record::start()
{
	auto& md = md_ptr->md;
	CThostFtdcDepthMarketDataField t{};
	while (true)
	{
		if (md.remove(t))
		{
            static auto day = md_ptr->mdapi->GetTradingDay();
            char fileName[64] = { '\0' };
            sprintf(fileName, "./%s/%s_%s.csv", day, t.InstrumentID, day);
            std::ofstream outFile;
            outFile.open(fileName, std::ios::app); // 文件追加写入
            outFile << t.InstrumentID << ","///交易所代码
                << t.ExchangeID << ","///合约代码
                << t.TradingDay << ","///交易日
                << t.UpdateTime << ","///最后修改时间
                << t.UpdateMillisec << ","///最后修改毫秒
                << t.PreSettlementPrice << ","///上次结算价
                << t.PreClosePrice << ","///昨收盘
                << t.PreOpenInterest << ","///昨持仓量
                << t.OpenPrice << ","///今开盘
                << t.UpperLimitPrice << ","///涨停板价
                << t.LowerLimitPrice << ","///跌停板价
                << t.HighestPrice << ","///最高价
                << t.LowestPrice << ","///最低价
                << t.LastPrice << ","///最新价
                << t.Volume << ","///数量
                << t.Turnover << ","///成交金额
                << t.OpenInterest << ","///持仓量
                << t.BidPrice1 << ","///申买价一
                << t.BidVolume1 << ","///申买量一
                << t.AskPrice1 << ","///申卖价一
                << t.AskVolume1 << ","///申卖量一
                << t.BidPrice2 << ","///申买价二
                << t.BidVolume2 << ","///申买量二
                << t.AskPrice2 << ","///申卖价二
                << t.AskVolume2 << ","///申卖量二
                << t.BidPrice3 << ","///申买价三
                << t.BidVolume3 << ","///申买量三
                << t.AskPrice3 << ","///申卖价三
                << t.AskVolume3 << ","///申卖量三
                << t.BidPrice4 << ","///申买价四
                << t.BidVolume4 << ","///申买量四
                << t.AskPrice4 << ","///申卖价四
                << t.AskVolume4 << ","///申卖量四
                << t.BidPrice5 << ","///申买价五
                << t.BidVolume5 << ","///申买量五
                << t.AskPrice5 << ","///申卖价五
                << t.AskVolume5///申卖量五
                << std::endl;
            outFile.close();
        }
	}
}
