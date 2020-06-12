#ifndef _CLINKEDLIST_H
#define _CLINKEDLIST_H
#define STACK_SIZE 5
#include "Stack.h"

template <typename T>
class CNode
{
	T	mData;
	CNode<T>*	mNext;

public:

	CNode()
	{
		memset(&mData, 0, sizeof(mData));
		mNext = nullptr;
	}

	CNode(T _data) : mData(_data), mNext(nullptr){}
	~CNode(){}

	T GetmData()
	{
		return mData;
	}

	CNode<T>*  GetmNext()
	{
		return mNext;
	}

	void SetmData(T _data)
	{
		mData=_data;
	}
	void SetmNext(CNode<T>* _next)
	{
		mNext=_next;
	}
	
};

template<typename T>
class CLinkedList
{
	CNode<T>*	mHead;
	CNode<T>*	mTail;

	CStack<CNode<T>*, 5>* mSearchStack;
	CNode<T>*	mSearchNode;

	int			mCount;

public:
	CLinkedList();
	~CLinkedList();

	CNode<T>*  GetmHead(){ return mHead; }
	CNode<T>*  GetmTail(){ return mTail; }

	bool	Insert(T  _data);
	bool	Delete(T  _data);
	int		GetCount();
	
	void	SearchStart();
	T		SearchData();
	void	SearchEnd();
};

template <typename T>
void CLinkedList<T>::SearchStart()
{
	mSearchStack->push(mSearchNode);
	mSearchNode = nullptr;
}


template <typename T>
T CLinkedList<T>::SearchData()
{
	if (mSearchNode == nullptr)
	{
		mSearchNode = GetmHead();
	}

	mSearchNode = mSearchNode->GetmNext();

	if (mSearchNode != nullptr)
	{
		return mSearchNode->GetmData();
	}

	return nullptr;
	
}

template <typename T>
void CLinkedList<T>::SearchEnd()
{
	mSearchStack->pop(mSearchNode);
}

template <typename T>
CLinkedList<T>::CLinkedList()
{
	mHead = new CNode<T>();
	mTail = mHead;
	mSearchNode = nullptr;
	mSearchStack = new CStack<CNode<T>*, STACK_SIZE>();
	mCount = 0;
}


template <typename T>
CLinkedList<T>::~CLinkedList()
{
	CNode<T>* ptr = mHead->GetmNext();
	delete mHead;

	while (ptr != NULL)
	{
		mHead = ptr;
		ptr = ptr->GetmNext();
		delete mHead;
	}
}

template <typename T>
bool CLinkedList<T>::Insert(T _data)
{
	CNode<T>* ptr = new CNode<T>(_data);
	mTail->SetmNext(ptr);
	mTail = ptr;
	mCount++;
	return true;
}

template <typename T>
bool CLinkedList<T>::Delete(T _data)
{
	CNode<T>* pre = mHead;
	CNode<T>* ptr = pre->GetmNext();
	bool flag = false;

	while (ptr != NULL)
	{
		if (ptr->GetmData() == _data)
		{
			flag = true;
			if (mSearchNode == ptr)
			{
				mSearchNode = pre;
			}
			pre->SetmNext(ptr->GetmNext());
			if (ptr->GetmNext() == NULL)
			{
				mTail = pre;
			}
			delete ptr;
			break;
		}

		pre = ptr;
		ptr = ptr->GetmNext();

	}

	if (flag)
	{
		
		mCount--;
		return true;
	}

	return false;

}

template <typename T>
int CLinkedList<T>::GetCount()
{
	return mCount;
}






#endif