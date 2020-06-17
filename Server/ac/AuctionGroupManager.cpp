#include "AuctionGroupManager.h"

_AuctionGroupManager* _AuctionGroupManager::mpts = nullptr;
_AuctionGroupManager* _AuctionGroupManager::Create()
{
	if (mpts == nullptr)
	{
		mpts = new _AuctionGroupManager();
	}
	return mpts;
}

_AuctionGroupManager* _AuctionGroupManager::GetInstance()
{
	return mpts;
}

void _AuctionGroupManager::Destroy()
{
	if (mpts != nullptr)
	{
		delete mpts;
	}
}

_AuctionGroupManager::_AuctionGroupManager()
{
	Auction_Group_List = nullptr;
	Auction_Group_List = new CLinkedList< _AuctionGroupInfo*>();
}
_AuctionGroupManager::~_AuctionGroupManager()
{
	Auction_Group_List->SearchStart();
	while (1)
	{
		_AuctionGroupInfo* _info = Auction_Group_List->SearchData();

		if (_info != nullptr)
		{
			Auction_Group_List->Delete(_info);
		}
		else
		{
			Auction_Group_List->SearchEnd();
			Auction_Group_List = nullptr;
			break;
		}
	}
}

_AuctionGroupInfo* _AuctionGroupManager::SearchAuctionGroupInfo(_AuctionInfo* _auction_info)
{
	_AuctionGroupInfo* group_info = nullptr;
	Auction_Group_List->SearchStart();

	while (1)
	{
		group_info = Auction_Group_List->SearchData();
		if (group_info == nullptr)
		{			
			break;
		}

		if (group_info->GetAuctionInfo() == _auction_info)
		{			
			break;
		}
	}

	Auction_Group_List->SearchEnd();

	return group_info;
}

_AuctionGroupInfo* _AuctionGroupManager::AddAuctionGroupInfo(_AuctionInfo* _info)
{	
	EnterCriticalSection(&cs);
	_AuctionGroupInfo* group_info=nullptr;
	
	bool flag = false;
	Auction_Group_List->SearchStart();

	while (1)
	{
		group_info = Auction_Group_List->SearchData();
		if (group_info == nullptr)
		{			
			break;
		}

		if (group_info->GetAuctionInfo()==_info)
		{
			if (group_info->isfull())
			{
				Auction_Group_List->SearchEnd();
				LeaveCriticalSection(&cs);
				return nullptr;
			}
			
			flag = true;
			break;
		}
	}

	Auction_Group_List->SearchEnd();

	if (!flag)
	{		
		group_info = new _AuctionGroupInfo(_info);

		Auction_Group_List->Insert(group_info);
	}		
	LeaveCriticalSection(&cs);
	return group_info;
}

void _AuctionGroupManager::RemoveAuctionGroupInfo(_AuctionGroupInfo* _group_info)
{
	EnterCriticalSection(&cs);
	_AuctionGroupInfo* group_info = nullptr;
	Auction_Group_List->SearchStart();

	while (1)
	{
		group_info = Auction_Group_List->SearchData();
		if (group_info == nullptr)
		{			
			break;
		}

		if (group_info == _group_info)
		{		
			Auction_Group_List->Delete(_group_info);
			delete _group_info;
			break;
		}
	}

	Auction_Group_List->SearchEnd();
	LeaveCriticalSection(&cs);
}



bool _AuctionGroupManager::CheckAuctionCompleteGroup(_ClientInfo* _info)
{
	_AuctionGroupInfo* group_info = SearchAuctionGroupInfo(_info->GetTryInfo()->info);

	group_info->userlist_delete(_info);

	if (group_info->get_user_count() == 0)
	{
		Auction_Group_List->Delete(group_info);
		return true;
	}   	

	return false;
}

void _AuctionGroupManager::ExitAuctionGroup(_ClientInfo* _info)
{
	if (_info->GetTryInfo()->info != nullptr && _info->GetTryInfo()->info->GetState()!=AUCTION_COMPLETE)
	{
		_AuctionGroupInfo* group_info = SearchAuctionGroupInfo(_info->GetTryInfo()->info);
		
		group_info->userlist_delete(_info);
		
		if (group_info->GetMaxBidUser() == _info)
		{
			group_info->clear_timer_count();

			group_info->ChangeMaxBidUser();

			group_info->searchstart();
			while (1)
			{
				_ClientInfo* client_info;
				if (!group_info->searchdata(client_info))
				{
					break;
				}

				//client_info->pre_state = client_info->state;
				client_info->SetState('p', client_info->GetState('n'));
				//client_info->state = USER_EXIT_INFO_SEND_STAE;
				client_info->SetState('n', USER_EXIT_INFO_SEND_STAE);
			}

			group_info->searchend();

		}
	}

}

bool _AuctionGroupManager::TryBid(_ClientInfo* _info, int _price)
{
	_AuctionGroupInfo* group_info = SearchAuctionGroupInfo(_info->GetTryInfo() ->info);
	
	_ClientInfo* user = group_info->GetMaxBidUser();
		   	
	if (user == nullptr)
	{
		if (group_info->GetAuctionInfo()->GetProductPrice() > _price)
		{
			return false;
		}

		_info->GetTryInfo()->try_price = _price;
		group_info->UpdateMaxBidUser(_info);
		return true;
	}

	if (user->GetTryInfo()->try_price > _price)
	{
		return false;
	}

	_info->GetTryInfo()->try_price = _price;
	group_info->UpdateMaxBidUser(_info);
	return true;
}

CLinkedList<_AuctionGroupInfo*>* _AuctionGroupManager::GetGroupList()
{
	return Auction_Group_List;
}