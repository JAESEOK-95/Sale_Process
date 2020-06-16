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
	CLinkedList<_User_Info*>* Join_List;

public:
	static 	CRITICAL_SECTION cs;
	static _ClientManager* Create();
	static _ClientManager* GetInstance();
	static void Destroy();
	void RemoveClient(_ClientInfo*);
	_ClientInfo* AddClient(SOCKET, SOCKADDR_IN);
	int MessageRecv(_ClientInfo*);
	int MessageSend(_ClientInfo*);
	int PacketRecv(_ClientInfo*);
};

#endif // !CLIENTMANAGER_H
