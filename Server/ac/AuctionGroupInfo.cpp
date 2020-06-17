#include "AuctionGroupInfo.h"

_AuctionGroupInfo::_AuctionGroupInfo(_AuctionInfo* _info)
{
	auction_info = _info;
	user_list = new CLinkedList<_ClientInfo*>();

	bid_user = new CStack<_ClientInfo*, MAX_BID_COUNT>();

	now_auction_count = 0;
}

_AuctionGroupInfo::~_AuctionGroupInfo()
{
	if (user_list != nullptr)
	{
		delete user_list;
	}
}

bool _AuctionGroupInfo::userlist_insert(_ClientInfo* _info)
{

	if (user_list->GetCount() == auction_info->GetUserCount())
	{
		return false;
	}

	user_list->Insert(_info);

	return true;
}

bool _AuctionGroupInfo::userlist_delete(_ClientInfo* _info)
{

	return user_list->Delete(_info);
}

bool _AuctionGroupInfo::isfull()
{
	if (user_list->GetCount() == auction_info->GetUserCount())
	{
		return true;
	}

	return false;
}

void _AuctionGroupInfo::searchstart()
{
	user_list->SearchStart();
}

bool _AuctionGroupInfo::searchdata(_ClientInfo*& _info)
{
	_ClientInfo* info = user_list->SearchData();
	if (info == nullptr)
	{
		return false;
	}

	_info = info;
	return true;
}

void _AuctionGroupInfo::searchend()
{
	user_list->SearchEnd();
}

void _AuctionGroupInfo::clear_timer_count()
{
	now_auction_count = 0;
}

void _AuctionGroupInfo::increase_timer_count()
{
	now_auction_count += 1;
}

bool _AuctionGroupInfo::check_timer_count()
{
	if (now_auction_count % 10 == 0)
	{
		return true;
	}
	return false;
}

int _AuctionGroupInfo::get_timer_count()
{
	return now_auction_count / 10;
}

int _AuctionGroupInfo::get_user_count()
{
	return user_list->GetCount();
}


_AuctionInfo* _AuctionGroupInfo::GetAuctionInfo()
{
	return auction_info;
}
bool _AuctionGroupInfo::UpdateMaxBidUser(_ClientInfo* _info)
{	
	return bid_user->push(_info);
}

bool _AuctionGroupInfo::ChangeMaxBidUser()
{
	_ClientInfo* client_info;

	if (!bid_user->pop(client_info))
	{
		return false;
	}

	if (!bid_user->copytop(client_info))
	{
		return false;
	}

	return true;
}

_ClientInfo* _AuctionGroupInfo::GetMaxBidUser()
{
	_ClientInfo* client_info;
	if (!bid_user->copytop(client_info))
	{
		return nullptr;
	}
	return client_info;
}


