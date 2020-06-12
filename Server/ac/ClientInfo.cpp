#include "ClientInfo.h"
_ClientInfo::_ClientInfo()
{
	temp_user = new _User_Info();
	memset(temp_user, 0, sizeof(_User_Info));

	pre_state = INIT_STATE;
	state = INIT_STATE;
	loginstate = false;

	recvbytes = 0;
	comp_recvbytes = 0;
	r_sizeflag = false;

	sendbytes = 0;
	comp_sendbytes = 0;

	memset(recv_buf, 0, sizeof(recv_buf));
	memset(send_buf, 0, sizeof(send_buf));

	try_auction = new _Try_AuctionInfo;
	memset(try_auction, 0, sizeof(_Try_AuctionInfo));

}

_ClientInfo::~_ClientInfo()
{
	if (temp_user != nullptr)
	{
		delete temp_user;
	}
	if (try_auction != nullptr)
	{
		delete try_auction;
	}
}

void _ClientInfo::login(_User_Info* _info)
{
	loginstate = true;
	memset(temp_user, 0, sizeof(_User_Info));
	userinfo = _info;
}

void _ClientInfo::logout()
{
	loginstate = false;
	userinfo = nullptr;
}

void _ClientInfo::auction_start(_AuctionInfo* _info)
{
	try_auction->info = _info;
	try_auction->result = AUCTION_FAIL;
}

void _ClientInfo::auction_end()
{	
	try_auction->info = nullptr;
	try_auction->result = NODATA;
	try_auction->try_price = 0;
}

void _ClientInfo::try_auction_success()
{
	try_auction->result = AUCTION_SUCCESS;
	userinfo->auction_money -= try_auction->try_price;
}

bool _ClientInfo::check_auction_success()
{

	if (try_auction->result == AUCTION_SUCCESS)
	{
		return true;
	}

	return false;
}

bool _ClientInfo::check_money(int _price)
{
	if (userinfo->auction_money < _price)
	{
		return false;
	}

	return true;
}

void _ClientInfo::deposit(int _price)
{
	userinfo->auction_money += _price;
}

bool _ClientInfo::check_send_complete()
{
	if (sendbytes == 0)
	{
		return true;
	}

	return false;
}

bool _ClientInfo::is_disconnected()
{
	if (state == DISCONNECTED_STATE)
	{
		return true;
	}
	return false;
}