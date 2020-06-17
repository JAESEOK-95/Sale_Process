#pragma once
#include "Global.h"
#include "AuctionInfo.h"


struct _Try_AuctionInfo
{
	_AuctionInfo* info;
	int			 try_price;
	int          result;
};

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

	_Try_AuctionInfo* try_auction;


public:

	_ClientInfo(SOCKET,SOCKADDR_IN);							//생성자 == add 추가
	~_ClientInfo();						//소멸자

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
	SOCKET GetSock();
	char* Getbuf(char);		//sendbuf,recvbuf set Get fuc.		s = send, r = recv
	SOCKADDR_IN GetAddr();
	STATE GetState(char);		//pre,nomal flag  p = pre_state, n = state
	int Getbytes(char);
	int GetCompbytes(char); //sendbuf,recvbuf set Get fuc.		s = send, r = recv	
	bool GetReSizeflag();		//r_sizeflag Return
	_User_Info* GetUserInfo();
	_User_Info* GetTempUserInfo();
	_Try_AuctionInfo* GetTryInfo();				//Pick AuctionInfo Return
	void SetState(char,STATE);		//pre,nomal flag  p = pre_state, n = state
	void Setbytes(char, int);			//sendbuf,recvbuf set Get fuc.		s = send, r = recv
	void SetCompbytes(char, int);	//sendbuf,recvbuf set Get fuc.		s = send, r = recv
	void SetReSizeflag(bool);			// r_sizeflag
	//paket,.unpaket
	void GetProtocol(char* _ptr, PROTOCOL& _protocol);
	void PackPacket(char* _buf, PROTOCOL _protocol, int& _size);
	void PackPacket(char* _buf, PROTOCOL _protocol, int _data, int& _size);
	void PackPacket(char* _buf, PROTOCOL _protocol, int _data, char* _str1, int& _size);
	void PackPacket(char* _buf, PROTOCOL _protocol, CLinkedList<_AuctionInfo*>* _list, int& _size);
	void PackPacket(char* _buf, PROTOCOL _protocol, char* _str1, int& _size);
	void UnPackPacket(char* _buf, char* _str1, char* _str2, char* _str3, int& _data);
	void UnPackPacket(char* _buf, char* _str1, char* _str2);
	void UnPackPacket(char* _buf, int& _data1);
	void UnPackPacket(char* _buf, int& _data1, int& _data2);
	void UnPackPacket(char* _buf, _AuctionInfo& _info);
	///
};
