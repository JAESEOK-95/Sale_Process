#ifndef LOGINMANAGER_H
#define LOGINMANAGER_H


#include "ClientInfo.h"


class _LoginManager
{
private:
	static _LoginManager* mpts;
	CLinkedList<_User_Info*>* Join_List;
	_LoginManager();
	~_LoginManager();

public:

	static LoginManager* create();
	static LoginManager* GetInstance();
	static void Destroy();


};

#endif // !LOGINMANAGER_H
