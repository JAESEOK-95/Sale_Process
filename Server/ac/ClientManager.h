#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H



#include "ClientInfo.h"

class _ClientManager
{
private: 
	static _ClientManager* mpts;			//Manager pts
	_ClientManager();						// create
	~_ClientManager();						//delete

	CLinkedList<_ClientInfo*>* User_List;


public:

	static _ClientManager* Create();
	static _ClientManager* GetInstance();
	static void Destroy();
	CLinkedList<_User_Info*>* GetJoinList();
	CLinkedList<_ClientInfo*>* GetUserList();
	void RemoveClient(_ClientInfo*);
	_ClientInfo* AddClient(SOCKET, SOCKADDR_IN);
	int MessageRecv(_ClientInfo*);
	int MessageSend(_ClientInfo*);
	int PacketRecv(_ClientInfo*);
	_ClientInfo* SearchClientInfo(const char*);
};

#endif // !CLIENTMANAGER_H
