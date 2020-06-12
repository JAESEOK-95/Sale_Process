#include "AuctionInfo.h"
#include "ClientInfo.h"

_AuctionInfo::_AuctionInfo()
{
	memset(this, 0, sizeof(_AuctionInfo));
}

_AuctionInfo::~_AuctionInfo()
{
	
}


void _AuctionInfo::AuctionComplete(_User_Info* _info, int _price)
{
	max_bid_user = _info;
	auction_price = _price;
	auction_state = AUCTION_COMPLETE;
}
