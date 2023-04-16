#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <fstream>

#ifdef _WIN32
#include <filesystem>
#include <stdlib.h>
#else
#include<unistd.h>
#endif // _WIN32

#include <stdio.h>
#include <cstring>
#include <string>
#include <map>
#include <time.h>
#include <thread>
#include <chrono>

const char pMdFlowPath[] = "./mdflow/";
const char pTradeFlowPath[] = "./tdflow/";

struct BrokerInfo
{
	char TradeFront[32];
	char MarketFront[32];
	char BrokerID[16];
	char UserID[16];
	char Password[16];
	char AuthCode[32];
	char AppID[32];
};

inline void loadBrokerInfo(const char* filename, BrokerInfo& bi)
{
	FILE* fp = fopen(filename, "r");
	if (!fp) { perror("File opening failed"); return; }

	char buf[64] = { '\0' };
	char key[16] = { '\0' };
	char value[32] = { '\0' };
	std::map<std::string, std::string> m;

	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		if (char* r = strchr(buf, '\n')) { r[0] = '\0'; }
		sscanf(buf, "%s = %s", key, value);
		m[key] = value;
	}
	strcpy(bi.TradeFront, m["TradeFront"].c_str());
	strcpy(bi.MarketFront, m["MarketFront"].c_str());
	strcpy(bi.BrokerID, m["BrokerID"].c_str());
	strcpy(bi.UserID, m["UserID"].c_str());
	strcpy(bi.Password, m["Password"].c_str());
	strcpy(bi.AuthCode, m["AuthCode"].c_str());
	strcpy(bi.AppID, m["AppID"].c_str());
}

inline void createFolder(const char *path)
{
#ifdef _WIN32
	std::filesystem::path _path(path);
	if (std::filesystem::exists(_path))
	{
		std::filesystem::remove_all(_path);
	}
	std::filesystem::create_directories(_path);
#else
	if (access(path, F_OK) == 0)
	{
		char rm[32]; sprintf(rm, "rm %s -rf", path);
		system(rm);
	}
	char mkdir[32]; sprintf(mkdir, "mkdir %s", path);
	system(mkdir);
#endif // _WIN32
}

inline const char* reqRtnReason(int rtn)
{
	switch (rtn)
	{
	case 0:///发送成功
		return "Sent successfully"; break;
	case -1:///因网络原因发送失败
		return "Failed to send"; break;
	case -2:///未处理请求队列总数量超限
		return "Unprocessed requests exceeded the allowed volume"; break;
	case -3:///每秒发送请求数量超限
		return "The number of requests sent per second exceeds the allowed volume"; break;
	default:///未知
		return "Unknown";
	}
}

inline const char* nReason2str(int nReason)
{
	switch (nReason)
	{
	case 0x1001:
		return "网络读失败";
	case 0x1002:
		return "网络写失败";
	case 0x2001:
		return "接收心跳超时";
	case 0x2002:
		return "发送心跳失败";
	case 0x2003:
		return "收到错误报文";
	default:
		return "Unknown";
	}
}

inline void send2WeCom(const char* text)
{
	char command[512];
#ifdef _WIN32
	sprintf(command, "curl \"https://qyapi.weixin.qq.com/cgi-bin/webhook/send?key=6ef63fbe-ff08-4099-9a2c-4433c464a035\" -H \"Content-Type: application/json\" -d \"{\\\"msgtype\\\": \\\"text\\\",\\\"text\\\": {\\\"content\\\": \\\"Win %s\\\"}}\"", text);
#else
	sprintf(command, "curl \'https://qyapi.weixin.qq.com/cgi-bin/webhook/send?key=6ef63fbe-ff08-4099-9a2c-4433c464a035\' -H \'Content-Type: application/json\' -d \'{\"msgtype\": \"text\",\"text\": {\"content\": \"%s\"}}\'", text);
#endif
	system(command);
}

inline void overnight(const char* day)
{
	long long sleeptime = 0;
	std::time_t begin = std::time(nullptr);

	auto t = std::localtime(&begin);
	std::tm end = *t;
	end.tm_sec = 0; end.tm_min = 58; end.tm_hour = 8;

	if (t->tm_hour < 8)
	{
		if (t->tm_wday == 6)
		{
			sleeptime = mktime(&end) + 2 * 24 * 3600 - begin;
		}
		else
		{
			sleeptime = mktime(&end) - begin;
		}
		send2WeCom("sleep");
		std::this_thread::sleep_for(std::chrono::seconds(sleeptime));
	}
	else if (t->tm_hour < 20 && t->tm_hour > 14)
	{
#ifndef _WIN32
		char command[128] = { '\0' };
		sprintf(command, "tar -czvf %s.tar.gz %s && bypy upload %s.tar.gz && rm %s -rf", day, day, day, day);
		system(command);
#endif // _WIN32
		send2WeCom("restart");
		system("shutdown -r");
	}
}

#endif
