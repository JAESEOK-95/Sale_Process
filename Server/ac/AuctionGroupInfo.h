#pragma once
#include "Global.h"
#include "ClientInfo.h"

class  _AuctionGroupInfo
{
private:

	_AuctionInfo* auction_info;
	CLinkedList<_ClientInfo*>* user_list;

	int		now_auction_count;		//manager

	CStack<_ClientInfo*, MAX_BID_COUNT>* bid_user;

public:
	_AuctionGroupInfo(_AuctionInfo* _info);


	~_AuctionGroupInfo();


	bool userlist_insert(_ClientInfo* _info);


	bool userlist_delete(_ClientInfo* _info);


	bool isfull();


	void searchstart();

	bool searchdata(_ClientInfo*& _info);


	void searchend();

	void increase_timer_count();

	void clear_timer_count();
	int get_timer_count();

	_AuctionInfo* GetAuctionInfo();
	bool check_timer_count();

	int get_user_count();



	bool UpdateMaxBidUser(_ClientInfo* _info);


	bool ChangeMaxBidUser();


	_ClientInfo* GetMaxBidUser();

};
