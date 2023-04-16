#ifndef RECORD_H
#define RECORD_H

#include "Trader.h"
#include "MarketData.h"


class record
{
public:
	record();
	~record();

public:
	void start();

private:
	Trader* td_ptr;
	MarketData* md_ptr;	
};

#endif