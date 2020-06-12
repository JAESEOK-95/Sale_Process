#pragma once
#include "Global.h"

struct _AuctionInfo
{
	char	auction_product[PRODUCT_NAMESIZE];
	int		auction_product_code;
	int		auction_price;
	int		auction_user_count;
	int     auction_state;

	_User_Info* max_bid_user;

	_AuctionInfo();

	~_AuctionInfo();

	void AuctionComplete(_User_Info*, int);

};
