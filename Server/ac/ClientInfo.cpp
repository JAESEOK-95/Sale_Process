#include "ClientInfo.h"



// 기존 AddClient 대체 작업 
_ClientInfo::_ClientInfo(SOCKET _sock,SOCKADDR_IN _addr)
{
	temp_user = new _User_Info();
	memset(temp_user, 0, sizeof(_User_Info));
	printf("\nClient 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(_addr.sin_addr),
		ntohs(_addr.sin_port));


	sock = _sock;
	loginstate = false;

	recvbytes = 0;
	comp_recvbytes = 0;
	r_sizeflag = false;

	sendbytes = 0;
	comp_sendbytes = 0;
	SetState('p', INIT_STATE);
	SetState('n', INIT_STATE);

	memcpy(&addr, &_addr, sizeof(SOCKADDR_IN));
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

SOCKET _ClientInfo::GetSock()
{
	return sock;
}

SOCKADDR_IN _ClientInfo::GetAddr()
{
	return addr;
}

STATE _ClientInfo::GetState(char _flag)
{
	switch (_flag)
	{

	case 'p':
		return pre_state;
		break;
	case 'n':
		return state;
		break;
	}
}
int _ClientInfo::Getbytes(char _flag)
{
	switch (_flag)
	{
	case 's':
		return sendbytes;
		break;
	case 'r':
		return recvbytes;
		break;
	}
}

int _ClientInfo::GetCompbytes(char _flag)
{
	switch (_flag)
	{
	case 's':
		return comp_sendbytes;
		break;
	case 'r':
		return comp_recvbytes;
		break;
	}
}


char* _ClientInfo::Getbuf(char _flag)
{
	switch (_flag)
	{
	case 'r':
		return recv_buf;
		break;
	case 's':
		return send_buf;
		break;
	}
}

bool _ClientInfo::GetReSizeflag()
{
	return r_sizeflag;
}

_User_Info* _ClientInfo::GetUserInfo()
{
	return userinfo;
}
void _ClientInfo::SetState(char _flag,STATE _state)
{
	switch (_flag)
	{

	case 'p':
		pre_state = _state;
		break;
	case 'n':
		state = _state;
		break;
	}

}

void _ClientInfo::Setbytes(char _flag,int _num)
{
	switch (_flag)
	{
	case 's':
		sendbytes = _num;
		break;
	case 'r':
		recvbytes = _num;
		break;
	}
}

void _ClientInfo::SetCompbytes(char _flag, int _num)
{
	switch (_flag)
	{
	case 's':
		comp_sendbytes = _num;
		break;
	case 'r':
		comp_recvbytes = _num;
		break;
	}
}

void _ClientInfo::SetReSizeflag(bool _flag)
{
	r_sizeflag = _flag;
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

/*
* 기존 Server에서 paket전부 옮김
*
*
*/

void _ClientInfo::GetProtocol(char* _ptr, PROTOCOL& _protocol)
{
	memcpy(&_protocol, _ptr, sizeof(PROTOCOL));

}

void _ClientInfo::PackPacket(char* _buf, PROTOCOL _protocol, int& _size)
{
	char* ptr = _buf;
	_size = 0;

	ptr = ptr + sizeof(_size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	_size = _size + sizeof(_protocol);

	ptr = _buf;
	memcpy(ptr, &_size, sizeof(_size));

	_size = _size + sizeof(_size);

}

void _ClientInfo::PackPacket(char* _buf, PROTOCOL _protocol, int _data, int& _size)
{
	char* ptr = _buf;
	_size = 0;

	ptr = ptr + sizeof(_size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	_size = _size + sizeof(_protocol);

	memcpy(ptr, &_data, sizeof(_data));
	ptr = ptr + sizeof(_data);
	_size = _size + sizeof(_data);

	ptr = _buf;
	memcpy(ptr, &_size, sizeof(_size));

	_size = _size + sizeof(_size);

}

void _ClientInfo::PackPacket(char* _buf, PROTOCOL _protocol, int _data, char* _str1, int& _size)
{
	char* ptr = _buf;
	int strsize1 = strlen(_str1);
	_size = 0;

	ptr = ptr + sizeof(_size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	_size = _size + sizeof(_protocol);

	memcpy(ptr, &_data, sizeof(_data));
	ptr = ptr + sizeof(_data);
	_size = _size + sizeof(_data);

	memcpy(ptr, &strsize1, sizeof(strsize1));
	ptr = ptr + sizeof(strsize1);
	_size = _size + sizeof(strsize1);

	memcpy(ptr, _str1, strsize1);
	ptr = ptr + strsize1;
	_size = _size + strsize1;

	ptr = _buf;
	memcpy(ptr, &_size, sizeof(_size));

	_size = _size + sizeof(_size);
}

void _ClientInfo::PackPacket(char* _buf, PROTOCOL _protocol, CLinkedList<_AuctionInfo*>* _list, int& _size)
{
	char* ptr = _buf;
	_size = 0;
	int count = _list->GetCount();

	ptr = ptr + sizeof(_size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	_size = _size + sizeof(_protocol);

	char* countptr = ptr;
	ptr = ptr + sizeof(count);
	_size = _size + sizeof(count);

	_list->SearchStart();

	while (1)
	{
		_AuctionInfo* info = _list->SearchData();
		if (info == nullptr)
		{
			break;
		}

		if (info->auction_state == AUCTION_COMPLETE)
		{
			count--;
			continue;
		}

		memcpy(ptr, &info->auction_product_code, sizeof(info->auction_product_code));
		ptr = ptr + sizeof(info->auction_product_code);
		_size = _size + sizeof(info->auction_product_code);


		int namesize = strlen(info->auction_product);
		memcpy(ptr, &namesize, sizeof(namesize));
		ptr = ptr + sizeof(namesize);
		_size = _size + sizeof(namesize);

		memcpy(ptr, info->auction_product, namesize);
		ptr = ptr + namesize;
		_size = _size + namesize;

		memcpy(ptr, &info->auction_price, sizeof(info->auction_price));
		ptr = ptr + sizeof(info->auction_price);
		_size = _size + sizeof(info->auction_price);
	}

	_list->SearchEnd();

	memcpy(countptr, &count, sizeof(count));

	ptr = _buf;
	memcpy(ptr, &_size, sizeof(_size));

	_size = _size + sizeof(_size);

}

void _ClientInfo::PackPacket(char* _buf, PROTOCOL _protocol, char* _str1, int& _size)
{
	char* ptr = _buf;
	int strsize1 = strlen(_str1);
	_size = 0;

	ptr = ptr + sizeof(_size);

	memcpy(ptr, &_protocol, sizeof(_protocol));
	ptr = ptr + sizeof(_protocol);
	_size = _size + sizeof(_protocol);

	memcpy(ptr, &strsize1, sizeof(strsize1));
	ptr = ptr + sizeof(strsize1);
	_size = _size + sizeof(strsize1);

	memcpy(ptr, _str1, strsize1);
	ptr = ptr + strsize1;
	_size = _size + strsize1;

	ptr = _buf;
	memcpy(ptr, &_size, sizeof(_size));

	_size = _size + sizeof(_size);
}

void _ClientInfo::UnPackPacket(char* _buf, char* _str1, char* _str2, char* _str3, int& _data)
{
	int str1size, str2size, str3size;

	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&str1size, ptr, sizeof(str1size));
	ptr = ptr + sizeof(str1size);

	memcpy(_str1, ptr, str1size);
	ptr = ptr + str1size;

	memcpy(&str2size, ptr, sizeof(str2size));
	ptr = ptr + sizeof(str2size);

	memcpy(_str2, ptr, str2size);
	ptr = ptr + str2size;

	memcpy(&str3size, ptr, sizeof(str3size));
	ptr = ptr + sizeof(str3size);

	memcpy(_str3, ptr, str3size);
	ptr = ptr + str3size;

	memcpy(&_data, ptr, sizeof(_data));
	ptr = ptr + sizeof(_data);
}

void _ClientInfo::UnPackPacket(char* _buf, char* _str1, char* _str2)
{
	int str1size, str2size;

	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&str1size, ptr, sizeof(str1size));
	ptr = ptr + sizeof(str1size);

	memcpy(_str1, ptr, str1size);
	ptr = ptr + str1size;

	memcpy(&str2size, ptr, sizeof(str2size));
	ptr = ptr + sizeof(str2size);

	memcpy(_str2, ptr, str2size);
	ptr = ptr + str2size;
}

void _ClientInfo::UnPackPacket(char* _buf, int& _data1)
{
	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_data1, ptr, sizeof(_data1));
	ptr = ptr + sizeof(_data1);

}

void _ClientInfo::UnPackPacket(char* _buf, int& _data1, int& _data2)
{
	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&_data1, ptr, sizeof(_data1));
	ptr = ptr + sizeof(_data1);

	memcpy(&_data2, ptr, sizeof(_data2));
	ptr = ptr + sizeof(_data2);
}

void  _ClientInfo::UnPackPacket(char* _buf, _AuctionInfo& _info)
{
	int strsize;

	char* ptr = _buf + sizeof(PROTOCOL);

	memcpy(&strsize, ptr, sizeof(strsize));
	ptr = ptr + sizeof(strsize);

	memcpy(_info.auction_product, ptr, strsize);
	ptr = ptr + strsize;

	memcpy(&_info.auction_user_count, ptr, sizeof(_info.auction_user_count));
	ptr = ptr + sizeof(_info.auction_user_count);
}