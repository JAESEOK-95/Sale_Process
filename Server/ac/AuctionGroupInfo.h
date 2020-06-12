#pragma once
#include "Global.h"
#include "ClientInfo.h"

struct _AuctionGroupInfo
{
	_AuctionInfo* auction_info;
	CLinkedList<_ClientInfo*>* user_list;

	int		now_auction_count;

	CStack<_ClientInfo*, MAX_BID_COUNT>* bid_user;

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

	bool check_timer_count();

	int get_user_count();


	bool UpdateMaxBidUser(_ClientInfo* _info);


	bool ChangeMaxBidUser();


	_ClientInfo* GetMaxBidUser();

};
