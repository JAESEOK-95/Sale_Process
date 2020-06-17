#ifndef AUCTIONGROUPMANAGER_H
#define AUCTIONGROUPMANAGER_H

#include "AuctionGroupInfo.h"

class _AuctionGroupManager
{
private:
	static _AuctionGroupManager* mpts;
	CLinkedList<_AuctionGroupInfo*>* Auction_Group_List;

	_AuctionGroupManager();
	~_AuctionGroupManager();

public:
	static _AuctionGroupManager* Create();
	static _AuctionGroupManager* GetInstance();
	static void Destroy();

	CLinkedList<_AuctionGroupInfo*>* GetGroupList();
	_AuctionGroupInfo* SearchAuctionGroupInfo(_AuctionInfo* _auction_info);
	void RemoveAuctionGroupInfo(_AuctionGroupInfo*);
	_AuctionGroupInfo* AddAuctionGroupInfo(_AuctionInfo* _info);
	bool CheckAuctionCompleteGroup(_ClientInfo* _info);
	void ExitAuctionGroup(_ClientInfo* _info);
	bool TryBid(_ClientInfo* _info, int _price);
};
#endif // !AUCTIONGROUPMANAGER_H
