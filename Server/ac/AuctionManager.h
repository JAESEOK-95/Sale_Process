#ifndef AUCTIONMANAGER_H
#define AUTICONMANAGER_H



#include "AuctionInfo.h"

class _AuctionManager
{
private:
	static _AuctionManager* mpts;
	int Now_Product_Code;
	CLinkedList<_AuctionInfo*>* Auction_List;
	_AuctionManager();
	~_AuctionManager();
public:
	static _AuctionManager* Create();
	static _AuctionManager* GetInstance();
	static void Destroy();
	_AuctionInfo* AddAuctionInfo(const char*, int, int);
	void RemoveAuctionInfo(_AuctionInfo*);
	_AuctionInfo* SearchAuctionInfo(int);
	bool AllAuctionComplete();
};
#endif // !AUCTIONMANAGER_H
