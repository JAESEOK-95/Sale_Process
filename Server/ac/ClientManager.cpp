#include "ClientManager.h"


_ClientManager* _ClientManager::Create()
{

	if (mpts == nullptr)
	{
		mpts = new _ClientManager();
	}
	return mpts;
}


_ClientManager* _ClientManager::GetInstance()
{
	return mpts;
}

_ClientManager::_ClientManager()
{
	User_List = nullptr;


}

_ClientManager::~_ClientManager()
{

}

void _ClientManager::RemoveClient(_ClientInfo* _ptr)
{
	EnterCriticalSection(&cs);
	closesocket(_ptr->GetSock());

	printf("\nClient 종료: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(_ptr->GetAddr().sin_addr), ntohs(_ptr->GetAddr().sin_port));

	User_List->Delete(_ptr);
	delete _ptr;
	LeaveCriticalSection(&cs);
}

_ClientInfo* _ClientManager::AddClient(SOCKET _sock, SOCKADDR_IN _clientaddr)
{
	EnterCriticalSection(&cs);
	_ClientInfo* ptr = new _ClientInfo(_sock, _clientaddr);
	User_List->Insert(ptr);
	LeaveCriticalSection(&cs);
	return ptr;
}


int _ClientManager::MessageRecv(_ClientInfo* _info)
{
	int retval = recv(_info->GetSock(), _info->Getbuf('r') + _info->GetCompbytes('r'), _info->Getbytes('r') - _info->GetCompbytes('r') , 0);
	if (retval == SOCKET_ERROR) //강제연결종료요청인 경우
	{
		return ERROR_DISCONNECTED;
	}
	else if (retval == 0)
	{
		return DISCONNECTED;
	}
	else
	{
		int Temp = 0;
		Temp = _info->GetCompbytes('r');
		Temp += retval;
		_info->SetCompbytes('r', Temp);
		//_info->comp_recvbytes += retval;
		if (_info->GetCompbytes('r') == _info->Getbytes('r'))
		{

			_info->SetCompbytes('r', 0);
			_info->Setbytes('r', 0);
			return SOC_TRUE;
		}
		return SOC_FALSE;
	}

}

int _ClientManager::MessageSend(_ClientInfo* _info)
{
	int retval = send(_info->GetSock(), _info->Getbuf('s') + _info->GetCompbytes('s'),
		_info->Getbytes('s') - _info->GetCompbytes('s'), 0);
	if (retval == SOCKET_ERROR)
	{
		return ERROR_DISCONNECTED;
	}
	else if (retval == 0)
	{
		DISCONNECTED;
	}
	else
	{
		int Temp = 0;
		Temp = _info->GetCompbytes('s');
		Temp += retval;
		_info->SetCompbytes('s', Temp);

		if (_info->GetCompbytes('s') == _info->Getbytes('s'))
		{

			_info->SetCompbytes('s', 0);
			_info->Setbytes('s', 0);
			return SOC_TRUE;
		}
		else
		{
			return SOC_FALSE;
		}
	}
}

int _ClientManager::PacketRecv(_ClientInfo* _ptr)
{
	if (!_ptr->r_sizeflag)
	{
		//_ptr->recvbytes = sizeof(int);
		_ptr->Setbytes('r', sizeof(int));
		int retval = MessageRecv(_ptr);
		switch (retval)
		{
		case SOC_TRUE:
			memcpy(&_ptr->recvbytes, _ptr->recv_buf, sizeof(int));
			_ptr->r_sizeflag = true;
			return SOC_FALSE;
		case SOC_FALSE:
			return SOC_FALSE;
		case ERROR_DISCONNECTED:
			err_display("recv error()");
			return DISCONNECTED;
		case DISCONNECTED:
			return DISCONNECTED;
		}
	}

	int retval = MessageRecv(_ptr);
	switch (retval)
	{
	case SOC_TRUE:
		_ptr->r_sizeflag = false;
		return SOC_TRUE;
	case SOC_FALSE:
		return SOC_FALSE;
	case ERROR_DISCONNECTED:
		err_display("recv error()");
		return DISCONNECTED;
	case DISCONNECTED:
		return DISCONNECTED;
	}
}