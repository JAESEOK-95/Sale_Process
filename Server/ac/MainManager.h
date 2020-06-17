#ifndef MAINMANAGER_H
#define MAINMANAGER_H

#include "ClientManager.h"
#include "AuctionGroupManager.h"
#include "AuctionManager.h"

class MainManager
{
private:
	static MainManager* Mm_pts;
	static _ClientManager* Cm_pts;
	static _AuctionManager* Am_pts;
	static _AuctionGroupManager* Agm_pts;
	MainManager();
	~MainManager();

public:
	static MainManager* create();
	static MainManager* GetInstance();
	static void Destroy();
	static void Run();
	BOOL SearchFile(char* filename);
	bool FileDataLoad();
	bool FileDataAdd(_User_Info* _info);
	bool FileDataSave();
	static DWORD CALLBACK CountThread(LPVOID ptr);
};
#endif // !MAINMANAGER_H
