#include "AuctionManager.h"

_AuctionManager* _AuctionManager::mpts = nullptr;
_AuctionManager::_AuctionManager()
{
	Auction_List = nullptr;
	Now_Product_Code = 0;
	Auction_List = new CLinkedList<_AuctionInfo*>();
}

_AuctionManager::~_AuctionManager()
{

	Auction_List->SearchStart();

	while (1)
	{
		_AuctionInfo* _info = Auction_List->SearchData();

		if (_info != nullptr)
		{
			Auction_List->Delete(_info);
		}
		else
		{
			Auction_List->SearchEnd();
			Auction_List = nullptr;

			break;
		}

	}

}

_AuctionManager* _AuctionManager::Create()
{
	if (mpts == nullptr)
	{
		mpts = new _AuctionManager();
	}
	return mpts;
}

_AuctionManager* _AuctionManager::GetInstance()
{
	return mpts;
}

void _AuctionManager::Destroy()
{
	if (mpts != nullptr)
	{
		delete mpts;
	}
}


_AuctionInfo* _AuctionManager::AddAuctionInfo(const char* _product, int _count, int _price)
{
	Now_Product_Code += _count;
	_AuctionInfo* info = new _AuctionInfo(Now_Product_Code,_product, _count, _price);
	//_AuctionInfo* info = new _AuctionInfo(_product, _count, _price);
	Auction_List->Insert(info);
	return info;
}

void _AuctionManager::RemoveAuctionInfo(_AuctionInfo* _auctioninfo)
{
	 
	Auction_List->Delete(_auctioninfo);
	delete _auctioninfo;
	 
}


_AuctionInfo* _AuctionManager::SearchAuctionInfo(int _auction_code)
{
	_AuctionInfo* auction_info = nullptr;
	Auction_List->SearchStart();

	while (1)
	{
		auction_info = Auction_List->SearchData();
		if (auction_info == nullptr)
		{
			break;
		}

		if (auction_info->GetState() == AUCTION_COMPLETE)
		{
			continue;
		}

		if (auction_info->GetProductCode() == _auction_code)
		{
			break;
		}
	}

	Auction_List->SearchEnd();
	return auction_info;
}

bool _AuctionManager::AllAuctionComplete()
{
	Auction_List->SearchStart();

	while (1)
	{
		_AuctionInfo* auction_info = Auction_List->SearchData();
		if (auction_info == nullptr)
		{
			break;
		}

		if (auction_info->GetState() != AUCTION_COMPLETE)
		{
			return false;
		}
	}

	return true;
}

CLinkedList<_AuctionInfo*>* _AuctionManager::GetAuctionList()
{
	return Auction_List;
}