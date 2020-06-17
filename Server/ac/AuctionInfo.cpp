#include "AuctionInfo.h"
#include "ClientInfo.h"

_AuctionInfo::_AuctionInfo(const int _code,const char* _product, int _count, int _price)
{
	memset(this, 0, sizeof(_AuctionInfo));
	strcpy(auction_product, _product);
	auction_user_count = _count;
	auction_price = _price;
	auction_product_code = _code;
	auction_state = AUCTION_ONGOING;
}
_AuctionInfo::_AuctionInfo(const char* _product, int _count, int _price)
{
	memset(this, 0, sizeof(_AuctionInfo));
	strcpy(auction_product, _product);
	auction_user_count = _count;
	auction_price = _price;
	auction_product_code = auction_user_count++;
	auction_state = AUCTION_ONGOING;
}

_AuctionInfo::~_AuctionInfo()
{
	
}


char* _AuctionInfo::GetProductname()
{
	return auction_product;
}

int _AuctionInfo::GetProductCode()
{
	return auction_product_code;
}

int _AuctionInfo::GetProductPrice()
{
	return auction_price;
}

int _AuctionInfo::GetState()
{
	return auction_state;
}

int _AuctionInfo::GetUserCount()
{
	return auction_user_count;
}
void _AuctionInfo::AuctionComplete(_User_Info* _info, int _price)
{
	max_bid_user = _info;
	auction_price = _price;
	auction_state = AUCTION_COMPLETE;
}
