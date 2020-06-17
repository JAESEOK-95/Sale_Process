#include "ClientManager.h"

_ClientManager* _ClientManager::mpts = nullptr;
_ClientManager* _ClientManager::Create()
{

	if (mpts == nullptr)
	{
		mpts = new _ClientManager();
	}
	return mpts;
}

void _ClientManager::Destroy()
{
	if (mpts != nullptr)
	{
		delete mpts;
		mpts = nullptr;
	}
}

CLinkedList<_User_Info*>* _ClientManager::GetJoinList()
{
	return Join_List;
}
CLinkedList<_ClientInfo*>* _ClientManager::GetUserList()
{
	return User_List;
}

_ClientManager* _ClientManager::GetInstance()
{
	return mpts;
}

_ClientManager::_ClientManager()
{
	User_List = nullptr;
	Join_List = nullptr;

	User_List = new CLinkedList<_ClientInfo*>();
	Join_List = new CLinkedList<_User_Info*>();

}

_ClientManager::~_ClientManager()
{
	User_List->SearchStart();

	while (1)
	{
		_ClientInfo* _info = User_List->SearchData();
		if (_info != nullptr)
		{
			User_List->Delete(_info);
		}
		else
		{
			User_List->SearchEnd();
			break;
		}
	}

	Join_List->SearchStart();

	while (1)
	{
		_User_Info* _u_info = Join_List->SearchData();
		if (_u_info != nullptr)
		{
			Join_List->Delete(_u_info);
		}
		else
		{
			Join_List->SearchEnd();
			break;
		}
	}


}

void _ClientManager::RemoveClient(_ClientInfo* _ptr)
{
	 
	closesocket(_ptr->GetSock());

	printf("\nClient 종료: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(_ptr->GetAddr().sin_addr), ntohs(_ptr->GetAddr().sin_port));

	User_List->Delete(_ptr);
	delete _ptr;
	 
}

_ClientInfo* _ClientManager::AddClient(SOCKET _sock, SOCKADDR_IN _clientaddr)
{
	 
	_ClientInfo* ptr = new _ClientInfo(_sock, _clientaddr);
	User_List->Insert(ptr);
	 
	return ptr;
}

_ClientInfo* _ClientManager::SearchClientInfo(const char* _id)
{
	_ClientInfo* info = nullptr;

	User_List->SearchStart();

	while (1)
	{
		info = User_List->SearchData();
		if (info == nullptr)
		{
			break;
		}
		if (!strcmp(info->GetUserInfo()->id, _id))
		{
			break;
		}
	}

	User_List->SearchEnd();

	return info;
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
	int Tempbytes = 0;
	if (!_ptr->GetReSizeflag())
	{
		//_ptr->recvbytes = sizeof(int);
		_ptr->Setbytes('r', sizeof(int));
		int retval = MessageRecv(_ptr);
		switch (retval)
		{
		case SOC_TRUE:
			memcpy(&Tempbytes, _ptr->Getbuf('r'), sizeof(int));  //test 필요
			_ptr->Setbytes('r', Tempbytes);
			_ptr->SetReSizeflag(true);
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
		//_ptr->r_sizeflag = false;
		_ptr->SetReSizeflag(false);
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