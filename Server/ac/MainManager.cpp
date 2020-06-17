#include "MainManager.h"

MainManager* MainManager::Mm_pts = nullptr;

SOCKET hDummySock;		//mainmanager
HANDLE hEvent;
//시간제한 스레드
DWORD CALLBACK MainManager::CountThread(LPVOID ptr)
{
	char msg[BUFSIZE] = { 0 };
	int Temp = 0;
	SOCKADDR_IN dumyAddr;

	memset(&dumyAddr, 0, sizeof(dumyAddr));

	dumyAddr.sin_family = AF_INET;
	dumyAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	dumyAddr.sin_port = htons(9900);

	while (1)
	{
		int result = WaitForSingleObject(hEvent, 100);
		EnterCriticalSection(&cs);

		switch (result)
		{
		case WAIT_OBJECT_0:
			LeaveCriticalSection(&cs);
			continue;
		case WAIT_TIMEOUT:

			//Auction_Group_List->SearchStart();
			Agm_pts->GetInstance()->GetGroupList()->SearchStart();

			while (1)
			{

				_AuctionGroupInfo* group_info = Agm_pts->GetInstance()->GetGroupList()->SearchData();
				if (group_info == nullptr)
				{
					break;
				}

				//최고 입찰자가 있는지 검사
				if (group_info->GetMaxBidUser() == nullptr)
				{
					continue;
				}

				group_info->increase_timer_count();  //있으면 카운팅 시작 신호
				if (group_info->check_timer_count())
				{
					group_info->searchstart();

					while (1)
					{
						_ClientInfo* client_info = nullptr;
						if (!group_info->searchdata(client_info))
						{
							break;
						}


						//client_info->pre_state = client_info->state;
						client_info->SetState('p', client_info->GetState('n'));
						//client_info->state = AUCTION_COUNT_SEND_STATE;
						client_info->SetState('n', AUCTION_COUNT_SEND_STATE);
					}
					group_info->searchend();

					sendto(hDummySock, msg, sizeof(int), 0, (SOCKADDR*)&dumyAddr, sizeof(dumyAddr));
				}

			}

			Agm_pts->GetInstance()->GetGroupList()->SearchEnd();
		}

		LeaveCriticalSection(&cs);


	}
}

BOOL MainManager::SearchFile(char* filename)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile = FindFirstFile(filename, &FindFileData);
	if (hFindFile == INVALID_HANDLE_VALUE)
		return FALSE;
	else {
		FindClose(hFindFile);
		return TRUE;
	}
}

bool MainManager::FileDataLoad()
{
	if (!SearchFile("userinfo.info"))
	{
		FILE* fp = fopen("userinfo.info", "wb");
		fclose(fp);
		return true;
	}

	FILE* fp = fopen("userinfo.info", "rb");
	if (fp == NULL)
	{
		return false;
	}

	_User_Info info;
	memset(&info, 0, sizeof(_User_Info));

	while (1)
	{
		fread(&info, sizeof(_User_Info), 1, fp);
		if (feof(fp))
		{
			break;
		}
		_User_Info* ptr = new _User_Info;
		memcpy(ptr, &info, sizeof(_User_Info));
		Cm_pts->GetInstance()->GetJoinList()->Insert(ptr);
	}

	fclose(fp);
	return true;
}

bool MainManager::FileDataAdd(_User_Info* _info)
{
	FILE* fp = fopen("userinfo.info", "ab");
	if (fp == NULL)
	{
		return false;
	}

	int retval = fwrite(_info, 1, sizeof(_User_Info), fp);

	if (retval != sizeof(_User_Info))
	{
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

bool MainManager::FileDataSave()
{
	FILE* fp = fopen("userinfo.info", "wb");
	if (fp == NULL)
	{
		return false;
	}

	//Join_List->SearchStart();
	Cm_pts->GetInstance()->GetJoinList()->SearchStart();

	while (1)
	{
		_User_Info* info = Cm_pts->GetInstance()->GetJoinList()->SearchData();
		if (info == nullptr)
		{
			break;
		}
		int retval = fwrite(info, 1, sizeof(_User_Info), fp);

		if (retval != sizeof(_User_Info))
		{
			fclose(fp);
			return false;
		}
	}

	Cm_pts->GetInstance()->GetJoinList()->SearchEnd();

	fclose(fp);
	return true;
}


MainManager::MainManager()
{

}

MainManager::~MainManager()
{

}

MainManager* MainManager::create()
{
	if (Mm_pts == nullptr)
	{
		Mm_pts = new MainManager();
		Cm_pts = nullptr;
		Am_pts = nullptr;
		Agm_pts = nullptr;
	}
	Cm_pts = _ClientManager::Create();
	Am_pts = _AuctionManager::Create();
	Agm_pts = _AuctionGroupManager::Create();

	return Mm_pts;
}

MainManager* MainManager::GetInstance()
{
	return Mm_pts;
}

void MainManager::Destroy()
{
	if (Mm_pts != nullptr)
	{
		Cm_pts->Destroy();
		Am_pts->Destroy();
		Agm_pts->Destroy();
		delete Mm_pts;
	}

}

void MainManager::Run()
{
	WSADATA wsaData;
	SOCKET hServSock;
	SOCKADDR_IN servAddr, dumyAddr;
	SOCKET hClntSock;
	SOCKADDR_IN clntAddr;
	int retval;
	FD_SET Rset, Wset;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) /* Load Winsock 2.2 DLL */
		err_quit("WSAStartup() error!");

	InitializeCriticalSection(&cs);

	hEvent = CreateEvent(nullptr, false, false, nullptr);

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
		err_quit("socket() error");

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(9000);

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		err_quit("bind() error");

	hDummySock = socket(PF_INET, SOCK_DGRAM, 0);
	if (hDummySock == INVALID_SOCKET)
		err_quit("socket() error");

	dumyAddr.sin_family = AF_INET;
	dumyAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	dumyAddr.sin_port = htons(9900);

	if (bind(hDummySock, (SOCKADDR*)&dumyAddr, sizeof(dumyAddr)) == SOCKET_ERROR)
		err_quit("bind() error");


	if (listen(hServSock, 5) == SOCKET_ERROR)
		err_quit("listen() error");

	if (!Mm_pts->GetInstance()->FileDataLoad())
	{
		err_quit("file read error!");
	}

	char msg[BUFSIZE];
	PROTOCOL protocol;
	int size;

	Am_pts->GetInstance()->AddAuctionInfo("식탁", 3, 100000);
	Am_pts->GetInstance()->AddAuctionInfo("침대", 2, 100000);
	Am_pts->GetInstance()->AddAuctionInfo("장식장", 3, 100000);
	Am_pts->GetInstance()->AddAuctionInfo("책상", 3, 100000);
	Am_pts->GetInstance()->AddAuctionInfo("액자", 3, 100000);

	CreateThread(NULL, 0, CountThread, NULL, 0, NULL);

	while (1)
	{
		FD_ZERO(&Rset);
		FD_ZERO(&Wset);

		FD_SET(hServSock, &Rset);
		FD_SET(hDummySock, &Rset);

		Cm_pts->GetInstance()->GetUserList()->SearchStart();

		while (1)
		{
			EnterCriticalSection(&cs);

			_ClientInfo* ptr = Cm_pts->GetInstance()->GetUserList()->SearchData();
			if (ptr == nullptr)
			{
				LeaveCriticalSection(&cs);
				break;
			}

			FD_SET(ptr->GetSock(), &Rset);

			switch (ptr->GetState('n'))
			{
			case JOIN_RESULT_SEND_STATE:
			case LOGIN_RESULT_SEND_STATE:
			case USER_INFO_SEND_STATE:
			case AUCION_MONEY_UPDATE_SEND_STATE:
			case LOGOUT_RESULT_SEND_STATE:
			case USER_EXIT_INFO_SEND_STAE:
			case AUCTION_LIST_SEND_STATE:
			case AUCTION_BID_WAIT_SEND_STATE:
			case AUCTION_BID_START_SEND_STATE:
			case AUCTION_BID_SEND_STATE:
			case AUCTION_COUNT_SEND_STATE:
			case AUCTION_COMPLETE_STATE:
			case CONNECT_END_SEND_STATE:
			case AUCTION_ERROR_SEND_STATE:
				FD_SET(ptr->GetSock(), &Wset);
				break;
			}

			LeaveCriticalSection(&cs);
		}

		Cm_pts->GetInstance()->GetUserList()->SearchEnd();

		if (select(0, &Rset, &Wset, 0, NULL) == SOCKET_ERROR)
		{
			err_quit("select() error");
		}


		if (FD_ISSET(hServSock, &Rset)) //새 큻ㄹ라가 들어오면 설정
		{
			int clntLen = sizeof(clntAddr);
			hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &clntLen);
			_ClientInfo* ptr = Cm_pts->GetInstance()->AddClient(hClntSock, clntAddr);
			ptr->SetState('n', LOGIN_MENU_SELECT_STATE);
			continue;
		}

		Cm_pts->GetInstance()->GetUserList()->SearchStart(); //유저 리스트 검색 시작

		while (1)
		{
			_ClientInfo* ptr = Cm_pts->GetInstance()->GetUserList()->SearchData();		// 1개씩 가ㅕ온다.

			if (ptr == nullptr)
			{
				break;
			}

			if (FD_ISSET(ptr->GetSock(), &Rset))
			{
				int result = Cm_pts->GetInstance()->PacketRecv(ptr);

				switch (result)
				{
				case DISCONNECTED:
					ptr->SetState('n', DISCONNECTED_STATE);
					break;
				case SOC_FALSE:
					continue;
				case SOC_TRUE:
					break;
				}

				ptr->GetProtocol(ptr->Getbuf('r'), protocol);

				if (ptr->GetState('n') != DISCONNECTED_STATE && protocol == CONNECT_END)
				{
					EnterCriticalSection(&cs);
					ptr->SetState('p', ptr->GetState('n'));
					ptr->SetState('n', CONNECT_END_SEND_STATE);

					LeaveCriticalSection(&cs);
					continue;
				}

				switch (ptr->GetState('n'))
				{
				case LOGIN_MENU_SELECT_STATE:
					EnterCriticalSection(&cs);

					switch (protocol)
					{
					case JOIN_INFO:
						memset(ptr->GetTempUserInfo(), 0, sizeof(_User_Info));
						ptr->UnPackPacket(ptr->Getbuf('r'), ptr->GetTempUserInfo()->id, 
								ptr->GetTempUserInfo()->pw, ptr->GetTempUserInfo()->nickname, ptr->GetTempUserInfo()->auction_money);
						ptr->SetState('n', JOIN_RESULT_SEND_STATE);
						break;
					case LOGIN_INFO:
						memset(ptr->GetTempUserInfo(), 0, sizeof(_User_Info));
						ptr->UnPackPacket(ptr->Getbuf('r'), ptr->GetTempUserInfo()->id, ptr->GetTempUserInfo()->pw);
					
						ptr->SetState('n', LOGIN_RESULT_SEND_STATE);
						break;
					}
					LeaveCriticalSection(&cs);
					break;
				case AUCTION_MENU_SELECT_STATE:
					EnterCriticalSection(&cs);
					switch (protocol)
					{

					case REQ_AUCTION_LIST_INFO:
						ptr->SetState('n', AUCTION_LIST_SEND_STATE);
						break;
					case AUCTION_SELECT_INFO:
					{
						int code = 0;
						ptr->UnPackPacket(ptr->Getbuf('r'), code);

						_AuctionInfo* auc_info = Am_pts->GetInstance()->SearchAuctionInfo(code);
						if (auc_info == nullptr)
						{
							ptr->GetTryInfo()->result = AUCTION_CODE_ERROR;

							ptr->SetState('p', ptr->GetState('n'));
							ptr->SetState('n', AUCTION_ERROR_SEND_STATE);

							break;
						}

						_AuctionGroupInfo* group_info = Agm_pts->GetInstance()->AddAuctionGroupInfo(auc_info);

						if (group_info == nullptr)
						{
							ptr->GetTryInfo()->result = AUCTION_GROUP_FULL_ERROR;
							ptr->SetState('p', ptr->GetState('n'));
							ptr->SetState('n', AUCTION_ERROR_SEND_STATE);
							break;
						}

						ptr->auction_start(auc_info);
						group_info->userlist_insert(ptr);
						ptr->SetState('n', AUCTION_BID_WAIT_SEND_STATE);
					}
					break;
					case DEPOSIT_INFO:
					{
						int price;
						ptr->UnPackPacket(ptr->Getbuf('r'), price);
						ptr->deposit(price);
						ptr->SetState('p', ptr->GetState('n'));
						ptr->SetState('n', AUCION_MONEY_UPDATE_SEND_STATE);

					}
					break;
					case LOGOUT:
						ptr->logout();
						ptr->SetState('n', LOGOUT_RESULT_SEND_STATE);
						break;
					}
					LeaveCriticalSection(&cs);
					break;

				case AUCTION_BID_STATE:
					EnterCriticalSection(&cs);
					{
						int price;
						_AuctionGroupInfo* group_info;

						switch (protocol)
						{
						case AUCTION_BID_PRICE:
							ptr->UnPackPacket(ptr->Getbuf('r') , price);

							if (!ptr->check_money(price))
							{
								ptr->GetTryInfo()->result = AUCTION_SHORT_OF_MONEY_ERROR;
								ptr->SetState('p', ptr->GetState('n'));
								ptr->SetState('n', AUCTION_ERROR_SEND_STATE);
								break;
							}

							if (!Agm_pts->GetInstance()->TryBid(ptr, price))
							{
								ptr->GetTryInfo()->result = AUCTION_BID_MONEY_ERROR;
								ptr->SetState('p', ptr->GetState('n'));
								ptr->SetState('n', AUCTION_ERROR_SEND_STATE);
								break;
							}

							group_info = Agm_pts->GetInstance()->SearchAuctionGroupInfo(ptr->GetTryInfo()->info);
							group_info->searchstart();

							while (1)
							{
								_ClientInfo* client_info = nullptr;
								if (!group_info->searchdata(client_info))
								{
									break;
								}
								client_info->SetState('p', client_info->GetState('n'));
								client_info->SetState('n', AUCTION_BID_SEND_STATE);
							}

							group_info->searchend();
							group_info->clear_timer_count();
							break;
						case LOGOUT:
							ptr->logout();
							ptr->SetState('n', LOGOUT_RESULT_SEND_STATE);
							break;
						}

					}
					LeaveCriticalSection(&cs);
					break;

				case DISCONNECTED_STATE:
					EnterCriticalSection(&cs);
					Agm_pts->GetInstance()->ExitAuctionGroup(ptr);
					Cm_pts->GetInstance()->RemoveClient(ptr);
					Mm_pts->GetInstance()->FileDataSave();
					LeaveCriticalSection(&cs);
					continue;
				}
			}

			if (FD_ISSET(ptr->GetSock(), &Wset))
			{
				int join_result = NODATA;
				int login_result = NODATA;
				int result;

				switch (ptr->GetState('n'))
				{
				case JOIN_RESULT_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						Cm_pts->GetInstance()->GetJoinList()->SearchStart();

						while (1)
						{
							_User_Info* user_info = Cm_pts->GetInstance()->GetJoinList()->SearchData();
							if (user_info == nullptr)
							{
								break;
							}
							if (!strcmp(user_info->id, ptr->GetTempUserInfo()->id))
							{
								join_result = ID_EXIST;
								strcpy(msg, ID_EXIST_MSG);
								break;
							}
						}

						Cm_pts->GetInstance()->GetJoinList()->SearchEnd();

						if (join_result == NODATA)
						{
							_User_Info* user = new _User_Info;
							memset(user, 0, sizeof(_User_Info));
							strcpy(user->id, ptr->GetTempUserInfo()->id);
							strcpy(user->pw, ptr->GetTempUserInfo()->pw);
							strcpy(user->nickname, ptr->GetTempUserInfo()->nickname);
							user->auction_money = ptr->GetTempUserInfo()->auction_money;

							Mm_pts->GetInstance()->FileDataAdd(user);

							Cm_pts->GetInstance()->GetJoinList()->Insert(user);
							join_result = JOIN_SUCCESS;
							strcpy(msg, JOIN_SUCCESS_MSG);
						}

						protocol = JOIN_RESULT;

						ptr->PackPacket(ptr->Getbuf('s'), protocol, join_result, msg, size);
						ptr->Setbytes('s', size);
					}

					result = Cm_pts->GetInstance()->MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->SetState('n', DISCONNECTED_STATE);
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ptr->SetState('n', LOGIN_MENU_SELECT_STATE);
					LeaveCriticalSection(&cs);
					break;
				case LOGIN_RESULT_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						Cm_pts->GetInstance()->GetJoinList()->SearchStart();
						while (1)
						{
							_User_Info* user_info = Cm_pts->GetInstance()->GetJoinList()->SearchData();
							if (user_info == nullptr)
							{
								break;
							}
							if (!strcmp(user_info->id, ptr->GetTempUserInfo()->id))
							{
								if (!strcmp(user_info->pw, ptr->GetTempUserInfo()->pw))
								{
									ptr->login(user_info);
									login_result = LOGIN_SUCCESS;
									strcpy(msg, LOGIN_SUCCESS_MSG);
								}
								else
								{
									login_result = PW_ERROR;
									strcpy(msg, PW_ERROR_MSG);
								}
								break;
							}
						}

						Cm_pts->GetInstance()->GetJoinList()->SearchEnd();
						if (login_result == NODATA)
						{
							login_result = ID_ERROR;
							strcpy(msg, ID_ERROR_MSG);
						}

						protocol = LOGIN_RESULT;

						ptr->PackPacket(ptr->Getbuf('s'), protocol, login_result, msg, size);
						ptr->Setbytes('s', size);
					}

					result = Cm_pts->GetInstance()->MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->SetState('n', DISCONNECTED_STATE);
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}

					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}

					if (login_result == LOGIN_SUCCESS)
					{
						ptr->SetState('n', USER_INFO_SEND_STATE);
					}
					else
					{
						ptr->SetState('n', LOGIN_MENU_SELECT_STATE);
					}
					LeaveCriticalSection(&cs);
					break;
				case USER_INFO_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						ptr->PackPacket(ptr->Getbuf('s'), USER_INFO,ptr->GetUserInfo()->auction_money, ptr->GetUserInfo()->nickname, size);
						ptr->Setbytes('s', size);
					}

					result = Cm_pts->GetInstance()->MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->SetState('n', DISCONNECTED_STATE);
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}

					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ptr->SetState('n', AUCTION_MENU_SELECT_STATE);
					LeaveCriticalSection(&cs);
					break;
				case AUCION_MONEY_UPDATE_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						ptr->PackPacket(ptr->Getbuf('s'), AUCTION_MONEY_UPDATE_INFO, ptr->GetUserInfo()->auction_money, size);
						ptr->Setbytes('s', size);
					}

					result = Cm_pts->GetInstance()->MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->SetState('n', DISCONNECTED_STATE);
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}

					ptr->SetState('n', ptr->GetState('p'));
					LeaveCriticalSection(&cs);
					break;

				case AUCTION_LIST_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						if (Am_pts->GetInstance()->AllAuctionComplete())
						{
							ptr->PackPacket(ptr->Getbuf('s'), ALL_AUCTION_COMPLETE, AUCTION_ALL_AUCTION_COMPLETE_MSG, size);
						}
						else
						{
							ptr->PackPacket(ptr->Getbuf('s'), AUCTION_LIST_INFO, Am_pts->GetInstance()->GetAuctionList(), size);
						}
						ptr->Setbytes('s', size);
					}

					result = Cm_pts->GetInstance()->MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->SetState('n', DISCONNECTED_STATE);
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ptr->SetState('n', AUCTION_MENU_SELECT_STATE);
					LeaveCriticalSection(&cs);
					break;

				case AUCTION_BID_WAIT_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						ptr->PackPacket(ptr->Getbuf('s'), AUCTION_BID_WAIT, AUCTION_BID_WAIT_MSG, size);
						ptr->Setbytes('s', size);
					}

					result = Cm_pts->GetInstance()->MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->SetState('n', DISCONNECTED_STATE);
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ptr->SetState('n', AUCTION_BID_WAIT_STATE);
					LeaveCriticalSection(&cs);
				case AUCTION_BID_WAIT_STATE:
					EnterCriticalSection(&cs);
					{
						_AuctionGroupInfo* group_info = Agm_pts->GetInstance()->SearchAuctionGroupInfo(ptr->GetTryInfo()->info);
						if (group_info->isfull())
						{
							group_info->searchstart();
							while (1)
							{
								_ClientInfo* client_info = nullptr;
								if (!group_info->searchdata(client_info))
								{
									break;
								}
						
								client_info->SetState('n', AUCTION_BID_START_SEND_STATE);
							}
							group_info->searchend();

						}

					}
					LeaveCriticalSection(&cs);
					break;
				case AUCTION_BID_START_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						ptr->PackPacket(ptr->Getbuf('s'), AUCTION_BID_START, AUCTION_BID_START_MSG, size);
						ptr->Setbytes('s', size);
					}

					result = Cm_pts->GetInstance()->MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->SetState('n', DISCONNECTED_STATE);
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ptr->SetState('n', AUCTION_BID_STATE);
					LeaveCriticalSection(&cs);
					break;

				case AUCTION_BID_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						_AuctionGroupInfo* group_info = Agm_pts->GetInstance()->SearchAuctionGroupInfo(ptr->GetTryInfo()->info);
						memset(msg, 0, sizeof(msg));
						sprintf(msg, "%s님이 %d원을 입찰하셨습니다.\n", group_info->GetMaxBidUser()->GetUserInfo()->nickname,
							group_info->GetMaxBidUser()->GetTryInfo()->try_price);
						ptr->PackPacket(ptr->Getbuf('s'), AUCTION_BID_PRICE, msg, size);
						ptr->Setbytes('s', size);
					}

					result = Cm_pts->GetInstance()->MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->SetState('n', DISCONNECTED_STATE);
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ptr->SetState('n', ptr->GetState('p'));
					LeaveCriticalSection(&cs);
					break;

				case AUCTION_COUNT_SEND_STATE:  //최고 입찰가가 나오면 카운팅을 시작하여 5초간 타이머를 건다.
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						_AuctionGroupInfo* group_info = Agm_pts->GetInstance()->SearchAuctionGroupInfo(ptr->GetTryInfo()->info);
						sprintf(msg, "%d초 지났습니다. 5초가 지나면 경매가 종료됩니다.\n", group_info->get_timer_count());
						ptr->PackPacket(ptr->Getbuf('s'), AUCTION_SECOND_COUNT, msg, size);
						ptr->Setbytes('s', size);
					}

					result = Cm_pts->GetInstance()->MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->SetState('n', DISCONNECTED_STATE);
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}

					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}

					{
						_AuctionGroupInfo* group_info = Agm_pts->GetInstance()->SearchAuctionGroupInfo(ptr->GetTryInfo()->info);

						if (group_info->get_timer_count() >= MAX_AUCTION_COUNT)
						{
							if (group_info->GetMaxBidUser() == ptr)
							{
								ptr->try_auction_success();

								ptr->GetTryInfo()->info->AuctionComplete(ptr->GetUserInfo(), ptr->GetTryInfo()->try_price);
							}
							ptr->SetState('n', AUCTION_COMPLETE_STATE);
							LeaveCriticalSection(&cs);
							break;
						}
					}
					ptr->SetState('n', ptr->GetState('p'));
					LeaveCriticalSection(&cs);
					break;
				case AUCTION_COMPLETE_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						if (ptr->check_auction_success())
						{
							sprintf(msg, "%s물품에 대한 경매에 성공했습니다. 낙찰가는 %d원입니다.\n",
								ptr->GetTryInfo()->info->GetProductname(), ptr->GetTryInfo()->info->GetProductPrice());
						}
						else
						{
							sprintf(msg, "%s물품에 대한 경매에 실패했습니다. %s님에게 낙찰되었으며 낙찰가는 %d원입니다.\n",
								ptr->GetTryInfo()->info->GetProductname(),
								ptr->GetTryInfo()->info->GetMaxUser()->nickname,
								ptr->GetTryInfo()->info->GetProductPrice());
						}

						ptr->PackPacket(ptr->Getbuf('s'), AUCTION_RESULT_INFO, msg, size);
						ptr->Setbytes('s', size);
					}

					result = Cm_pts->GetInstance()->MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->SetState('n', DISCONNECTED_STATE);
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}

					if (Agm_pts->GetInstance()->CheckAuctionCompleteGroup(ptr))
					{
						//
					}

					if (ptr->GetTryInfo()->result == AUCTION_SUCCESS)
					{
						ptr->auction_end();
						ptr->SetState('p', AUCTION_MENU_SELECT_STATE);
						ptr->SetState('n', AUCION_MONEY_UPDATE_SEND_STATE);

					}
					else
					{
						ptr->auction_end();
						ptr->SetState('n', AUCTION_MENU_SELECT_STATE);
					}

					LeaveCriticalSection(&cs);
					break;
				case AUCTION_ERROR_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						memset(msg, 0, sizeof(msg));

						switch (ptr->GetTryInfo()->result)
						{
						case AUCTION_GROUP_FULL_ERROR:
							strcpy(msg, AUCTION_FULL_ERROR_MSG);
							break;
						case AUCTION_CODE_ERROR:
							strcpy(msg, AUCTION_CODE_ERROR_MSG);
							break;
						case AUCTION_BID_MONEY_ERROR:
							strcpy(msg, AUCTION_BID_MONEY_ERROR_MSG);
							break;
						case AUCTION_SHORT_OF_MONEY_ERROR:
							strcpy(msg, AUCTION_MONEY_ERROR_MSG);
							break;
						}
						ptr->PackPacket(ptr->Getbuf('s'), AUCTION_ERROR_INFO, msg, size);
						ptr->Setbytes('s', size);
					}

					result = Cm_pts->GetInstance()->MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->SetState('n', DISCONNECTED_STATE);
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}

					ptr->SetState('n', ptr->GetState('p'));
					LeaveCriticalSection(&cs);
					break;
				case CONNECT_END_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						ptr->PackPacket(ptr->Getbuf('s'), CONNECT_END, size);
						ptr->Setbytes('s', size);
					}

					result = Cm_pts->GetInstance()->MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->SetState('n', DISCONNECTED_STATE);
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ptr->SetState('n', ptr->GetState('p'));
					LeaveCriticalSection(&cs);
					break;
				case USER_EXIT_INFO_SEND_STAE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						ptr->PackPacket(ptr->Getbuf('s'), USER_EXIT_INFO, USER_EXIT_INFO_MSG, size);
						ptr->Setbytes('s', size);
					}

					result = Cm_pts->GetInstance()->MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->SetState('n', DISCONNECTED_STATE);
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					ptr->SetState('n', ptr->GetState('p'));
					LeaveCriticalSection(&cs);
					break;
				case LOGOUT_RESULT_SEND_STATE:
					EnterCriticalSection(&cs);
					if (ptr->check_send_complete())
					{
						ptr->PackPacket(ptr->Getbuf('s'), LOGOUT_RESULT, LOGOUT_MSG, size);
						ptr->Setbytes('s', size);
					}

					result = Cm_pts->GetInstance()->MessageSend(ptr);
					switch (result)
					{
					case ERROR_DISCONNECTED:
						err_display("connect end");
					case DISCONNECTED:
						ptr->SetState('n', DISCONNECTED_STATE);
						break;
					case SOC_FALSE:
						LeaveCriticalSection(&cs);
						continue;
					case SOC_TRUE:
						break;
					}
					if (ptr->is_disconnected())
					{
						LeaveCriticalSection(&cs);
						break;
					}
					Agm_pts->GetInstance()->ExitAuctionGroup(ptr);
					ptr->SetState('n', LOGIN_MENU_SELECT_STATE);
					LeaveCriticalSection(&cs);
					break;
				}//switch protocol end

				if (ptr->GetState('n') == DISCONNECTED_STATE)
				{
					Cm_pts->GetInstance()->RemoveClient(ptr);
					continue;
				}
			}//if wset end
		}// client service while end

		Cm_pts->GetInstance()->GetUserList()->SearchEnd();
	}//accept while end

	closesocket(hServSock);

	DeleteCriticalSection(&cs);

	WSACleanup();
}

