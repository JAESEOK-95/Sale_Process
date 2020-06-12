#pragma once
#include "Global.h"
#include "AuctionInfo.h"

class _ClientInfo
{
private:
	SOCKET							sock;
	SOCKADDR_IN					addr;
	_User_Info*						userinfo;
	_User_Info*						temp_user;
	STATE								pre_state;
	STATE								state;
	bool								loginstate;
	int									recvbytes;
	int									comp_recvbytes;
	bool								r_sizeflag;

	int									sendbytes;
	int									comp_sendbytes;

	char								recv_buf[BUFSIZE];
	char								send_buf[BUFSIZE];
public:

	_ClientInfo();
	~_ClientInfo();

};

struct _ClientInfo
{
	struct _Try_AuctionInfo
	{
		_AuctionInfo* info;
		int			 try_price;
		int          result;
	};



	_Try_AuctionInfo*				try_auction;



	

	void login(_User_Info* _info);
	
	void logout();
	

	void auction_start(_AuctionInfo* _info);
	
    void auction_end();
	
	void try_auction_success();
	

	bool check_auction_success();
	

	bool check_money(int _price);
	

	void deposit(int _price);
	

	bool check_send_complete();
	
	bool is_disconnected();
};
