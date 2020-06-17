#pragma once
#include "Global.h"

class _AuctionInfo
{
private:
	char	auction_product[PRODUCT_NAMESIZE];
	int		auction_product_code;	//main manager ·Î °£´Ù
	int		auction_price;
	int		auction_user_count;
	int     auction_state;

	_User_Info* max_bid_user;


public:
	_AuctionInfo(const char*, int, int);
	_AuctionInfo(const int ,const char*, int,int);
	~_AuctionInfo();

	char* GetProductname();
	int GetProductCode();
	int GetProductPrice();
	int GetUserCount();
	int GetState();

	void AuctionComplete(_User_Info*, int);

};
